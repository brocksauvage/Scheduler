/** @file libscheduler.h
 */

#ifndef LIBSCHEDULER_H_
#define LIBSCHEDULER_H_



/**
  Constants which represent the different scheduling algorithms
*/
typedef enum {FCFS = 0, SJF, PSJF, PRI, PPRI, RR} scheme_t;

typedef struct _job_t
{
  int running_time;
  int priority;
  int process_time;
  int arrival_time;
  int jresponse_time;
  int prev_time;
	int scheduled;
	int latency;
//  int initial_time;
  int pid;
} job_t;

int num_cores;
int num_jobs;
job_t **core_arr;

scheme_t type;
float turnaround_time;
float wait_time;
float response_time;
float total_latency;
void  scheduler_start_up               (int cores, scheme_t scheme);
int   scheduler_new_job                (int job_number, int time, int running_time, int priority);
int   scheduler_job_finished           (int core_id, int job_number, int time);
int   scheduler_quantum_expired        (int core_id, int time);
float scheduler_average_turnaround_time();
float scheduler_average_waiting_time   ();
float scheduler_average_response_time  ();
void  scheduler_clean_up               ();

void  scheduler_show_queue             ();

#endif /* LIBSCHEDULER_H_ */
