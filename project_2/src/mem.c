#include "mem.h"
#include "util.h"
#include <stdlib.h>

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
