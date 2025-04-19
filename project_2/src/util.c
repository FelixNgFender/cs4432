#include "util.h"
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

void *check_malloc(size_t size) {
  void *p = malloc(size);
  if (p == NULL) {
    handle_error("malloc");
  }
  return p;
}

void *check_realloc(void *ptr, size_t size) {
  void *p = realloc(ptr, size);
  if (p == NULL) {
    handle_error("realloc");
  }
  return p;
}

void get_time(struct timespec *tp) {
  assert(tp);
  if (clock_gettime(CLOCK_MONOTONIC, tp) == -1) {
    handle_error("Error getting start time");
  }
}

// Adapted from https://stackoverflow.com/a/68804612
void get_time_diff(const struct timespec *start, const struct timespec *end,
                   struct timespec *diff_out) {
  assert(start);
  assert(end);
  assert(diff_out);
  diff_out->tv_sec = end->tv_sec - start->tv_sec;
  diff_out->tv_nsec = end->tv_nsec - start->tv_nsec;
  if (diff_out->tv_nsec < 0) {
    diff_out->tv_nsec += 1000000000; // nsec/sec
    diff_out->tv_sec--;
  }
  return;
}

long strtol_or_exit(const char *nptr) {
  char *end;
  errno = 0;
  const long n = strtol(nptr, &end, 10);
  if (nptr == end || '\0' != *end ||
      ((LONG_MIN == n || LONG_MAX == n) && ERANGE == errno) || n > INT_MAX ||
      n < INT_MIN) {
    fprintf(stderr, "Invalid number: %s %ld\n", nptr, n);
    handle_error("Error converting string to integer");
  }
  return n;
}
