#include "mem.h"
#include "util.h"
#include <stdlib.h>

void *check_malloc(int size) {
  void *p = malloc(size);
  if (p == NULL) {
    handle_error("malloc");
  }
  return p;
}
