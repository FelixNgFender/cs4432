#include "policy.h"
#include <string.h>

const char *policy_to_str(SchedulingPolicy p) {
  switch (p) {
  case POLICY_FIFO:
    return "FIFO";
  case POLICY_SJF:
    return "SJF";
  case POLICY_RR:
    return "RR";
  default:
    return "UNKNOWN";
  }
}

SchedulingPolicy str_to_policy(const char *s) {
  if (strcmp(s, "FIFO") == 0) {
    return POLICY_FIFO;
  } else if (strcmp(s, "RR") == 0) {
    return POLICY_RR;
  } else if (strcmp(s, "SJF") == 0) {
    return POLICY_SJF;
  } else {
    return POLICY_UNKNOWN;
  }
}
