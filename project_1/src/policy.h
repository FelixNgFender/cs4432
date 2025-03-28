#ifndef POLICY_H
#define POLICY_H

typedef enum {
  POLICY_UNKNOWN,
  POLICY_FIFO,
  POLICY_RR,
  POLICY_SJF,
} SchedulingPolicy;

const char *policy_to_str(SchedulingPolicy p);
SchedulingPolicy str_to_policy(const char *s);

#endif // POLICY_H
