#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

void *check_malloc(size_t size);
void *check_realloc(void *ptr, size_t size);
void get_time(struct timespec *tp);
void get_time_diff(const struct timespec *start, const struct timespec *end,
                   struct timespec *diff_out);
long strtol_or_exit(const char *nptr);

#endif // UTIL_H
