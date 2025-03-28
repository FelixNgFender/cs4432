#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

// Appendix B, Section B1, Little Book of Semaphores
void *check_malloc(int size) {
  void *p = malloc(size);
  if (p == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }
  return p;
}
