#include "buffer_manager.h"
#include "mem.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <buffer_pool_size>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  char *buffer_pool_size_str = argv[1];
  const long buffer_pool_size = strtol_or_exit(buffer_pool_size_str);
  if (buffer_pool_size < 0 || buffer_pool_size > UINT8_MAX) {
    fprintf(stderr, "Buffer pool size must be between 0 and %d\n", UINT8_MAX);
    exit(EXIT_FAILURE);
  }

  BufferManager *bm = check_malloc(sizeof(BufferManager));
  buffer_manager_init(bm, buffer_pool_size);
  buffer_manager_start(bm);
  buffer_manager_cleanup(bm);
  free(bm);

  return EXIT_SUCCESS;
}
