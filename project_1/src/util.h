#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

long strtol_or_exit(const char *nptr);

#endif // UTIL_H
