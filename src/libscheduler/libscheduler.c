/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

priqueue_t q;
/**
  Stores information making up a job to be scheduled including any statistics.
  You may need to define some global variables or a struct to store your job queue elements.
*/

/**
	A globally declared structure that contains all of the variables to be used
	when calculating the metrics for the scheduler.
*/
scheduler_metrics_t s;

int FCFS_COMPARE(const void * a, const void * b)
{
	return (1);
}
int SJF_COMPARE(const void * a, const void * b)
{
	return ( (*(job_t*)a).process_time - (*(job_t*)b).process_time );
}
int PRI_COMPARE(const void * a, const void * b)
{
  int compare = ( (*(job_t*)a).priority - (*(job_t*)b).priority );

  if(compare == 0)
  {
    return (*(job_t *)a).arrival_time - (*(job_t *)b).arrival_time;

  }
  else
  {
    return(compare);

  }
}


/**
  Initalizes the scheduler.
  Assumptions:DIAGRAM:at cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.
  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  s.type = scheme;

  s.num_cores = cores;
  s.turnaround_time = 0.0;
  s.wait_time = 0.0;
  s.response_time = 0.0;

  s.num_jobs = 0;

  s.core_arr = malloc(cores * sizeof(job_t));

  int i;
  for (i = 0; i < cores; i++)
  {
    s.core_arr[i] = NULL;
  }

  if (s.type == FCFS || s.type == RR)
  {
    priqueue_init(&q, FCFS_COMPARE);
  }
  else if (s.type == SJF || s.type == PSJF)
  {
    priqueue_init(&q, SJF_COMPARE);
  }
  else if (s.type == PRI || s.type == PPRI)
  {
    priqueue_init(&q, PRI_COMPARE);
  }
}


/**
  Called when a new job arrives.
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.
  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made.
 */
void longest_time_search(int time)
{
	s.longest_time = -1;
	s.longest_index = -1;
	for(int i = 0; i < s.num_cores; i ++)
	{
		s.core_arr[i]->process_time = s.core_arr[i]->process_time - (time - s.core_arr[i]->prev_time);
		s.core_arr[i]->prev_time = time;

		if(s.core_arr[i]->process_time > s.longest_time)
		{
			s.longest_time = s.core_arr[i]->process_time;
			s.longest_index = i;
		}
	}
}
void lowest_priority_search(int time)
{
	s.lowest_priority = s.core_arr[0]->priority;
	s.lowest_core = 0;
	for(int i =0; i < s.num_cores; i++)
	{

		if(s.core_arr[i]->priority > s.lowest_priority)
		{
			 s.lowest_priority = s.core_arr[i]->priority;
			 s.lowest_core = i;
		}
		else if(s.core_arr[i]->priority == s.lowest_priority)
		{
			if(s.core_arr[i]->arrival_time > s.core_arr[s.lowest_core]->arrival_time)
			{
			 s.lowest_core = i;
			}
		}
	}
}
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{

	job_t *new_job = malloc(s.num_cores * sizeof(job_t));
	new_job->pid = job_number;
	new_job->running_time = running_time;
	new_job->process_time = running_time;
	new_job->arrival_time = time;
	new_job->priority = priority;
	new_job->jresponse_time = 0;
	new_job->jresponse_time = -1;

  int idle_core;
  for(int i = 0; i < s.num_cores; i++)
  {
    if(s.core_arr[i] == NULL)
    {
      idle_core = i;
			break;
    }
    else
    {
      idle_core = -1;
    }
  }
  if(idle_core >= 0)
  {
		s.core_arr[idle_core] = new_job;
		s.core_arr[idle_core]->jresponse_time = time - s.core_arr[idle_core]->arrival_time;
		if(s.type == PSJF)
		{
			s.core_arr[idle_core]->prev_time = time;
		}
		return(idle_core);
  }
  else if(s.type == PSJF)
  {

		longest_time_search(time);
		if(new_job->process_time < s.longest_time)
		{
			if(s.core_arr[s.longest_index]->jresponse_time == (time - s.core_arr[s.longest_index]->arrival_time))
			{
				s.core_arr[s.longest_index]->jresponse_time = -1;
			}
			priqueue_offer(&q, s.core_arr[s.longest_index]);
			s.core_arr[s.longest_index] = new_job;
			if(new_job->jresponse_time == -1)
			{
				new_job->jresponse_time = time - s.core_arr[s.longest_index]->arrival_time;
			}

			return(s.longest_index);
		}
  }
  else if(s.type == PPRI)
  {

		lowest_priority_search(time);
	  if(new_job->priority < s.lowest_priority)
	  {

	   if(s.core_arr[s.lowest_core]->jresponse_time == time - s.core_arr[s.lowest_core]->arrival_time)
	   {
	     s.core_arr[s.lowest_core]->jresponse_time = -1;
	   }
     priqueue_offer(&q, s.core_arr[s.lowest_core]);
     s.core_arr[s.lowest_core] = new_job;
     if(s.core_arr[s.lowest_core]->jresponse_time == -1)
     {
      s.core_arr[s.lowest_core]->jresponse_time = time - s.core_arr[s.lowest_core]->arrival_time;
	   }

	    return s.lowest_core;
	  }
	}
	priqueue_offer(&q, new_job);
	return -1;
}


/**
  Called when a job has completed execution.
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
	job_t *curr_job = s.core_arr[core_id];

  s.wait_time += time - (curr_job->running_time) - (curr_job->arrival_time);
  s.turnaround_time += time - (curr_job->arrival_time);
	s.response_time += curr_job->jresponse_time;
  s.num_jobs++;


  free(curr_job);
  curr_job = NULL;

  if(priqueue_size(&q) != 0)
  {
		job_t *temp_job = (job_t*)priqueue_poll(&q);
		if(s.type == PSJF)
		{
			temp_job->prev_time = time;
		}

		if(temp_job->jresponse_time == -1)
		{
			temp_job->jresponse_time = time - temp_job->arrival_time;
		}
		s.core_arr[core_id] = temp_job;
		return(temp_job->pid);
  }
  else
  {
		s.core_arr[core_id] = NULL;
    return(-1);
  }
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.
  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	job_t* curr_job = s.core_arr[core_id];

	if(curr_job == NULL)
	{
		if(priqueue_size(&q) == 0)
		{
			return -1;
		}
	}
	else
	{
		priqueue_offer(&q, curr_job);
	}

	s.core_arr[core_id] = priqueue_poll(&q);

	if(s.core_arr[core_id]->jresponse_time == -1)
	{
		s.core_arr[core_id]->jresponse_time = time - s.core_arr[core_id]->arrival_time;
	}
	return (s.core_arr[core_id]->pid);
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{

	return(s.wait_time/s.num_jobs);
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return(s.turnaround_time/s.num_jobs);
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return(s.response_time/s.num_jobs);
}


/**
  Free any memory associated with your scheduler.
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  for(int i = 0; i < s.num_cores; i++)
  {
    if(s.core_arr != NULL)
    {
      free(s.core_arr[i]);
    }
  }
  free(s.core_arr);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:
    2(-1) 4(0) 1(-1)
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
	for(int i = 0; i < priqueue_size(&q); i++)
	{
		printf("%d ", ((job_t*)priqueue_at(&q, i))->pid);
	}
}
