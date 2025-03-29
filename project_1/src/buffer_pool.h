#ifndef BUFFER_POOL_H
#define BUFFER_POOL_H

#include "frame.h"

/**
 * Manages a pool of frames.
 */
typedef struct BufferPool {
  uint8_t num_frames;
  // Very cool feature of C99: flexible array member.
  // This allows structs to have a variable length array at runtime. The array
  // must be the last member of the struct.
  Frame frames[];
} BufferPool;

void buffer_pool_init(BufferPool *pool, uint8_t num_frames);
void print_buffer_pool(const BufferPool *pool);
/**
 * Get the frame index containing the specified `block_id` in the buffer pool.
 *
 * `block_id` is 1-indexed. Returns -1 if not found.
 */
int8_t buffer_pool_get_frame_idx(const BufferPool *pool, uint8_t block_id);
/**
 * Get the frame containing the specified `block_id` from the buffer pool.
 *
 * `block_id` is 1-indexed.
 */
const Frame *buffer_pool_get_frame(BufferPool *pool, uint8_t block_id);
/**
 * Get the mutable frame containing the specified `block_id` from the buffer
 * pool.
 *
 * `block_id` is 1-indexed.
 */
Frame *buffer_pool_get_frame_mutable(BufferPool *pool, uint8_t block_id);

#endif // BUFFER_POOL_H
