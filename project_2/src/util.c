#include "util.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

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
