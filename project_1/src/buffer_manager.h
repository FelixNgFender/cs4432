
#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "buffer_pool.h"
#include <stdint.h>

typedef struct BufferManager {
  BufferPool *pool;
} BufferManager;

/**
 * Set up the buffer pool with the given number of frames.
 */
void buffer_manager_init(BufferManager *bm, uint8_t buffer_pool_size);
/**
 * Begin reading instructions and completing requested operations. Loops
 * continuously. Returns when finished.
 */
void buffer_manager_start(BufferManager *bm);
/**
 * Free resources allocated for the buffer pool.
 */
void buffer_manager_cleanup(BufferManager *bm);

#endif
