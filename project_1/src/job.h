#ifndef JOB_H
#define JOB_H

#include "policy.h"

typedef struct JobData {
  int id;
  int length;
  int first_time_arrived;
  int time_arrived;
  int time_started;
  int time_waited;
  int time_left;
  int time_completed;
  int response_time;
  int turnaround_time;
} JobData;

typedef struct Job {
  struct JobData data;
  struct Job *next;
} Job;

void job_init(Job *job);
void job_set(Job *job, JobData data, Job *next);
void job_destroy(Job *job);
void jobs_destroy(Job *job);
int schedule_jobs(Job *job, SchedulingPolicy policy, int time_slice);
void print_job(Job *job);
void print_jobs(Job *job);
int parse_workload_from_file(Job *head, const char *filename);

#endif // JOB_H
