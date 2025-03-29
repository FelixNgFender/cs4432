#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

void *check_malloc(int size) {
  void *p = malloc(size);
  if (p == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  return p;
}
