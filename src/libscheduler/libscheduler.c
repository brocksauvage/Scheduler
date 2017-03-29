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

  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  type = scheme;

  num_cores = cores;
  turnaround_time = 0.0;
  wait_time = 0.0;
  response_time = 0.0;

  num_jobs = 0;

  core_arr = malloc(cores * sizeof(job_t));

  int i;
  for (i = 0; i < cores; i++)
  {
    core_arr[i] = NULL;
  }

  if (type == FCFS || type == RR)
  {
    priqueue_init(&q, FCFS_COMPARE);
  }
  else if (type == SJF || type == PSJF)
  {
    priqueue_init(&q, SJF_COMPARE);
  }
  else if (type == PRI || type == PPRI)
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
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
	job_t *new_job = malloc(num_cores * sizeof(job_t));

	new_job->pid = job_number;
	new_job->running_time = running_time;
	new_job->process_time = running_time;
	new_job->latency = 0;
	new_job->arrival_time = time;
	new_job->priority = priority;
	new_job->jresponse_time = -1;
	new_job->isRun = 0;
  int idle_core;

  for(int i = 0; i < num_cores; i++)
  {
    if(core_arr[i] == NULL)
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
		new_job->isRun = 1;
		core_arr[idle_core] = new_job;
		core_arr[idle_core]->jresponse_time = time - core_arr[idle_core]->arrival_time;
		if(type == PSJF)
		{
			core_arr[idle_core]->prev_time = time;
		}
		return(idle_core);
  }
  else if(type == PSJF)
  {
		int longest_time = -1;
		int longest_index = -1;
		for(int i = 0; i < num_cores; i ++)
		{
			core_arr[i]->process_time = core_arr[i]->process_time - (time - core_arr[i]->prev_time);
			core_arr[i]->prev_time = time;
			if(core_arr[i]->process_time > longest_time)
			{
				longest_time = core_arr[i]->process_time;
				longest_index = i;
			}

		}
		if(new_job->process_time < longest_time)
		{
			if(core_arr[longest_index]->jresponse_time == (time - core_arr[longest_index]->arrival_time))
			{
				printf("Reponse time = -1\n");
				core_arr[longest_index]->jresponse_time = -1;
			}
			priqueue_offer(&q, core_arr[longest_index]);
			if(new_job->jresponse_time == -1)
			{
				printf("Reponse time 1\n");
				new_job->jresponse_time = time - core_arr[longest_index]->arrival_time;
				new_job->latency = time - core_arr[longest_index]->arrival_time;
			}
			core_arr[longest_index] = new_job;
			return(longest_index);
		}
  }
  else if(type == PPRI)
  {
		int lowest_priority = core_arr[0]->priority;
		int lowest_core = 0;
		for(int i =0; i < num_cores; i++)
		{

			if(core_arr[i]->priority > lowest_priority)
	    {
	       lowest_priority = core_arr[i]->priority;
	       lowest_core = i;
	    }
	    else if(core_arr[i]->priority == lowest_priority)
	    {
				if(core_arr[i]->arrival_time > core_arr[lowest_core]->arrival_time)
				{
	       lowest_core = i;
			 	}
	    }
	  }

	  if(lowest_priority > new_job->priority)
	  {

	   if(core_arr[lowest_core]->jresponse_time == time - core_arr[lowest_core]->arrival_time)
	   {
	     core_arr[lowest_core]->jresponse_time = -1;
	   }
     priqueue_offer(&q, core_arr[lowest_core]);
     core_arr[lowest_core] = new_job;
     if(core_arr[lowest_core]->jresponse_time == -1)
     {
      core_arr[lowest_core]->jresponse_time = time - core_arr[lowest_core]->arrival_time;
	   }

	    return lowest_core;
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

	job_t *curr_job = core_arr[core_id];

  wait_time += time - (curr_job->running_time) - (curr_job->arrival_time);
  turnaround_time += time - (curr_job->arrival_time);
  response_time += curr_job->jresponse_time;
	total_latency += curr_job->latency;
	printf("%f", response_time);
  printf("\n");
  num_jobs++;

  free(curr_job);
  curr_job = NULL;

  if(priqueue_size(&q) != 0)
  {
		job_t *temp_job = (job_t*)priqueue_poll(&q);

		if(type == PSJF)
		{
			temp_job->prev_time = time;
		}
		/*if(temp_job->isRun == 0)
		{
			temp_job->isRun = 1;
			temp_job->jresponse_time = time - temp_job->arrival_time;
		}*/
		if(temp_job->jresponse_time == -1)
		{
			temp_job->jresponse_time = time - temp_job->arrival_time;
			printf("Reponse time \n");
		}
		core_arr[core_id] = temp_job;
		return(temp_job->pid);
  }
  else
  {
		core_arr[core_id] = NULL;
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
	job_t* curr_job = core_arr[core_id];

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
	core_arr[core_id] = priqueue_poll(&q);
	if(core_arr[core_id]->jresponse_time == -1)
	{
		core_arr[core_id]->jresponse_time = time - core_arr[core_id]->arrival_time;
		printf("Quantum: ");
		printf("%d", curr_job->jresponse_time);
		printf("\n");
	}

	return (core_arr[core_id]->pid);
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return(wait_time/num_jobs);
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return(turnaround_time/num_jobs);
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.
  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return(response_time/num_jobs);
}


/**
  Free any memory associated with your scheduler.

  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  for(int i = 0; i < num_cores; i++)
  {
    if(core_arr != NULL)
    {
      free(core_arr[i]);
    }
  }
  free(core_arr);
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
