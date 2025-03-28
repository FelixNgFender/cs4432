#include "job.h"
#include "mem.h"
#include "policy.h"
#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool is_job_first_started(Job *job);
bool is_job_completed(Job *job);
void print_schedule(Job *job);
void analyze_trace(Job *job);
void build_fifo_exec_trace(Job *job);
void build_sjf_exec_trace(Job *job);
void build_rr_exec_trace(Job *job, int time_slice);

int schedule_jobs(Job *job, SchedulingPolicy policy, int time_slice) {
  if (job == NULL) {
    fprintf(stderr, "No jobs to schedule\n");
    return -1;
  }

  const char *policy_str = policy_to_str(policy);
  switch (policy) {
  case POLICY_FIFO:
    build_fifo_exec_trace(job);
    printf("Execution trace with %s:\n", policy_str);
    print_schedule(job);
    printf("End of execution with %s.\n", policy_str);
    printf("Begin analyzing %s:\n", policy_str);
    analyze_trace(job);
    printf("End analyzing %s.\n", policy_str);
    break;
  case POLICY_SJF:
    build_sjf_exec_trace(job);
    printf("Execution trace with %s:\n", policy_str);
    print_schedule(job);
    printf("End of execution with %s.\n", policy_str);
    printf("Begin analyzing %s:\n", policy_str);
    analyze_trace(job);
    printf("End analyzing %s.\n", policy_str);
    break;
  case POLICY_RR:
    build_rr_exec_trace(job, time_slice);
    printf("Execution trace with %s:\n", policy_str);
    print_schedule(job);
    printf("End of execution with %s.\n", policy_str);
    printf("Begin analyzing %s:\n", policy_str);
    analyze_trace(job);
    printf("End analyzing %s.\n", policy_str);
    break;
  default:
    fprintf(stderr, "Unknown policy\n");
    return -1;
  }
  return 0;
}

void print_schedule(Job *job) {
  if (job == NULL) {
    return;
  }

  Job *cur = job;
  while (cur != NULL) {
    printf("Job %d ran for: %d\n", cur->data.id,
           cur->data.length - cur->data.time_left);
    cur = cur->next;
  }
}

void analyze_trace(Job *job) {
  if (job == NULL) {
    return;
  }

  Job *cur = job;
  int num_unique_jobs = 0;
  while (cur != NULL) {
    if (is_job_first_started(cur)) {
      num_unique_jobs++;
    }
    cur = cur->next;
  }

  cur = job;
  // 2-way hash map to map job id to order and vice versa
  // fcfs is first-come-first-serve
  JobData fcfs_job_data[num_unique_jobs];
  int job_id_to_order[num_unique_jobs];
  int order_idx = 0;
  int total_response_time = 0;
  int total_turnaround_time = 0;
  int total_wait_time = 0;

  while (cur != NULL) {
    if (is_job_first_started(cur)) {
      total_response_time += cur->data.response_time;
      // jobs are reported in the order of when first starting work on them, but
      // ONLY on the data of their final node
      fcfs_job_data[order_idx] = cur->data;
      job_id_to_order[cur->data.id] = order_idx;
      order_idx++;
    }

    if (is_job_completed(cur)) {
      total_turnaround_time += cur->data.turnaround_time;
      total_wait_time += cur->data.time_waited;
      // update job with data from last node
      int order_idx = job_id_to_order[cur->data.id];
      fcfs_job_data[order_idx] = cur->data;
    }
    cur = cur->next;
  }

  for (int i = 0; i < num_unique_jobs; i++) {
    printf("Job %d -- Response time: %d  Turnaround: %d  Wait: %d\n",
           fcfs_job_data[i].id, fcfs_job_data[i].response_time,
           fcfs_job_data[i].turnaround_time, fcfs_job_data[i].time_waited);
  }

  printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n",
         (float)total_response_time / num_unique_jobs,
         (float)total_turnaround_time / num_unique_jobs,
         (float)total_wait_time / num_unique_jobs);
}

int parse_workload_from_file(Job *head, const char *filename) {
  FILE *workload_fp = fopen(filename, "rb");
  if (workload_fp == NULL) {
    perror("Error opening workload file");
    return -1;
  }

  int id = 0;
  char job_len_line[256];
  Job *cur = head;
  Job *prev = NULL;
  while (fgets(job_len_line, sizeof(job_len_line), workload_fp) != NULL) {
    // trim newline character
    job_len_line[strcspn(job_len_line, "\r\n")] = '\0';
    if (strlen(job_len_line) == 0) {
      break;
    }
    const long job_len = strtol_or_exit(job_len_line);
    if (job_len <= 0) {
      fclose(workload_fp);
      fprintf(stderr, "Workload must be a positive integer\n");
      return -1;
    }

    // PERF: can optimize malloc in a hot loop
    job_set(cur,
            (JobData){
                .id = id,
                .length = job_len,
                .time_arrived = 0,
                .time_started = -1,
                .time_waited = 0,
                .time_left = job_len,
                .time_completed = -1,
            },
            NULL);
    cur->next = check_malloc(sizeof(Job));
    job_init(cur->next);
    prev = cur;
    cur = cur->next;
    id++;
  }

  fclose(workload_fp);
  if (prev == NULL) {
    fprintf(stderr, "Workload does not contain any tasks\n");
    return -1;
  }
  prev->next = NULL;
  job_destroy(cur);
  return 0;
}

void job_init(Job *job) {
  if (job == NULL) {
    return;
  }

  job->data = (JobData){
      .id = -1,
      .length = -1,
      .first_time_arrived = -1,
      .time_arrived = -1,
      .time_started = -1,
      .time_waited = -1,
      .time_left = -1,
      .time_completed = -1,
      .response_time = -1,
      .turnaround_time = -1,
  };
  job->next = NULL;
}

void job_set(Job *job, JobData data, Job *next) {
  if (job == NULL) {
    return;
  }

  job->data = data;
  job->next = next;
}

void job_destroy(Job *job) {
  if (job == NULL) {
    return;
  }

  free(job);
}

void jobs_destroy(Job *job) {
  if (job == NULL) {
    return;
  }

  // NOTE: Interesting observation - We have to free the next job first before
  // freeing the current job. This is because the next job is a part of the
  // current job. If we free the current job first, we lose the reference to
  // the next job and we can't free it.
  if (job->next != NULL) {
    jobs_destroy(job->next);
  }

  job_destroy(job);
}

// Whether this job is the very first time it starts in the job list
bool is_job_first_started(Job *job) {
  if (job == NULL) {
    return false;
  }
  return job->data.time_started ==
         job->data.first_time_arrived + job->data.time_waited;
}

// Whether this job, when completed, is the last time it appears in the job list
bool is_job_completed(Job *job) {
  if (job == NULL) {
    return false;
  }
  return job->data.time_left <= 0 && job->data.time_completed >= 0;
}

void print_job(Job *job) {
  if (job == NULL) {
    return;
  }

  printf("Job %d: length %d, first time arrived: %d, time arrived: %d, time "
         "started: %d, time waited: "
         "%d, time left: %d, time completed: %d, response time: %d, turnaround "
         "time: %d\n",
         job->data.id, job->data.length, job->data.first_time_arrived,
         job->data.time_arrived, job->data.time_started, job->data.time_waited,
         job->data.time_left, job->data.time_completed, job->data.response_time,
         job->data.turnaround_time);
}

void print_jobs(Job *job) {
  if (job == NULL) {
    return;
  }

  print_job(job);
  print_jobs(job->next);
}

void build_fifo_exec_trace(Job *job) {
  if (job == NULL) {
    return;
  }

  int cur_time = 0;
  Job *cur = job;
  while (cur != NULL) {
    cur->data.time_waited += cur_time - cur->data.time_arrived;
    cur->data.time_started = cur_time;
    cur->data.first_time_arrived = cur->data.time_arrived;
    cur->data.response_time = cur->data.time_waited;
    int time_worked = cur->data.time_left;
    cur_time += time_worked;
    cur->data.time_left -= time_worked;
    cur->data.time_completed = cur_time;
    cur->data.turnaround_time =
        cur->data.time_completed - cur->data.first_time_arrived;
    cur = cur->next;
  }
}

void swap(Job *a, Job *b) {
  JobData temp = a->data;
  a->data = b->data;
  b->data = temp;
}

// essentially bubble sort
void build_sjf_exec_trace(Job *job) {
  if (job == NULL) {
    return;
  }

  int swapped;
  Job *ptr1;
  Job *lptr = NULL;

  do {
    swapped = 0;
    ptr1 = job;

    while (ptr1->next != lptr) {
      if (ptr1->data.length > ptr1->next->data.length) {
        swap(ptr1, ptr1->next);
        swapped = 1;
      }
      ptr1 = ptr1->next;
    }
    lptr = ptr1;
  } while (swapped);

  int cur_time = 0;
  Job *cur = job;
  while (cur != NULL) {
    cur->data.time_waited += cur_time - cur->data.time_arrived;
    cur->data.time_started = cur_time;
    cur->data.first_time_arrived = cur->data.time_arrived;
    cur->data.response_time = cur->data.time_waited;
    int time_worked = cur->data.time_left;
    cur_time += time_worked;
    cur->data.time_left -= time_worked;
    cur->data.time_completed = cur_time;
    cur->data.turnaround_time =
        cur->data.time_completed - cur->data.first_time_arrived;
    cur = cur->next;
  }
}

void build_rr_exec_trace(Job *job, int time_slice) {
  if (job == NULL) {
    return;
  }

  Job *cur = job;
  Job *end = job;

  while (end->next != NULL) {
    end = end->next;
  }

  int cur_time = 0;
  while (cur != NULL) {
    if (cur->data.time_left <= 0) {
      fprintf(stderr, "[WARN]: Detected badly scheduled tasks:\n");
      return;
    }

    cur->data.time_waited += cur_time - cur->data.time_arrived;
    cur->data.time_started = cur_time;
    if (is_job_first_started(cur)) {
      cur->data.first_time_arrived = cur->data.time_arrived;
      cur->data.response_time = cur->data.time_waited;
    }
    int time_worked = cur->data.time_left;
    if (time_worked > time_slice) {
      time_worked = time_slice;
    }
    cur_time += time_worked;
    cur->data.time_left -= time_worked;
    if (cur->data.time_left > 0) {
      // add to end of list
      end->next = check_malloc(sizeof(Job));
      job_init(end->next);
      job_set(end->next,
              (JobData){
                  .id = cur->data.id,
                  .length = cur->data.time_left,
                  .first_time_arrived = cur->data.first_time_arrived,
                  .time_arrived = cur_time,
                  .time_started = -1,
                  .time_waited = cur->data.time_waited,
                  .time_left = cur->data.time_left,
                  .time_completed = cur->data.time_completed,
                  .response_time = cur->data.response_time,
                  .turnaround_time = cur->data.turnaround_time,
              },
              NULL);
      end = end->next;
    } else {
      cur->data.time_completed = cur_time;
      cur->data.turnaround_time =
          cur->data.time_completed - cur->data.first_time_arrived;
    }
    cur = cur->next;
  }
}
