#include "buffer_manager.h"
#include "input.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>

void buffer_manager_init(BufferManager *bm, uint8_t buffer_pool_size) {
  if (bm == NULL || buffer_pool_size < 0 || buffer_pool_size > UINT8_MAX) {
    return;
  }

  BufferPool *pool =
      check_malloc(sizeof(BufferPool) + sizeof(Frame[buffer_pool_size]));
  buffer_pool_init(pool, buffer_pool_size);
  bm->pool = pool;
}

void buffer_manager_start(BufferManager *bm) {
  if (bm == NULL) {
    return;
  }
  char *line = NULL;
  while (1) {
    printf("The program is ready for the next command\n");

    if (input_getline(&line) < 1) {
      printf("End of file.\n");
      return;
    } else {
      int rc = input_next_instruction(line, bm->pool);
      if (rc == -1) {
        fprintf(stderr,
                "Error: An error occured while processing the command %s\n",
                line);
      }
    }
    free(line);
  }
}

void buffer_manager_cleanup(BufferManager *bm) {
  if (bm == NULL) {
    return;
  }

  // FMA field is freed along with the BufferPool struct
  free(bm->pool);
}
