#include "job.h"
#include "mem.h"
#include "policy.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <policy> <job trace> <time slice>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *policy_str = argv[1];
  char *job_trace = argv[2];
  char *time_slice_str = argv[3];

  SchedulingPolicy policy = str_to_policy(policy_str);
  if (policy == POLICY_UNKNOWN) {
    fprintf(stderr, "Invalid policy: %s. Must be one of FIFO, SJF, RR\n",
            policy_str);
    exit(EXIT_FAILURE);
  }
  const long time_slice = strtol_or_exit(time_slice_str);
  if (time_slice < 0) {
    fprintf(stderr, "Time slice must be a non-negative integer\n");
    exit(EXIT_FAILURE);
  }

  Job *head = check_malloc(sizeof(Job));
  job_init(head);
  if (parse_workload_from_file(head, job_trace) != 0) {
    fprintf(stderr, "Failed to parse workload from file\n");
    exit(EXIT_FAILURE);
  }

  if (schedule_jobs(head, policy, time_slice) != 0) {
    fprintf(stderr, "Failed to schedule jobs\n");
    exit(EXIT_FAILURE);
  }

  jobs_destroy(head);
  return EXIT_SUCCESS;
}
