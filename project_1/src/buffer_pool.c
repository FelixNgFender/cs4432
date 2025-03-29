#include "buffer_pool.h"
#include <stdio.h>
#include <string.h>

/**
 * Swap in requested block on disk into an empty frame in the buffer pool.
 * If there are no empty frames, a frame is evicted.
 *
 * `block_id` is 1-indexed.
 */
int8_t buffer_pool_swap_in_block(BufferPool *pool, uint8_t block_id);
/**
 * Find the index of empty frame in the buffer pool.
 *
 * Returns -1 if no empty frames are found.
 */
int8_t buffer_pool_find_free_frame_idx(const BufferPool *pool);
/**
 * Evict an unpinned frame from the buffer pool, effectively freeing it. Write
 * it to disk if dirty.
 */
int8_t buffer_pool_evict_frame(BufferPool *pool);

void buffer_pool_init(BufferPool *pool, uint8_t num_frames) {
  if (pool == NULL || num_frames == 0) {
    return;
  }

  pool->num_frames = num_frames;
  for (uint8_t i = 0; i < num_frames; i++) {
    frame_init(&pool->frames[i]);
  }
}

void print_buffer_pool(const BufferPool *pool) {
  if (pool == NULL) {
    return;
  }

  printf("Buffer pool:\n");
  for (uint8_t i = 0; i < pool->num_frames; i++) {
    printf("Frame %d: ", i + 1);
    print_frame(&pool->frames[i]);
  }
}

int8_t buffer_pool_get_frame_idx(const BufferPool *pool, uint8_t block_id) {
  if (pool == NULL || block_id == 0 || block_id > pool->num_frames) {
    return -1;
  }

  for (uint8_t i = 0; i < pool->num_frames; i++) {
    if (frame_get_block_id(&pool->frames[i]) == block_id) {
      return i;
    }
  }

  return -1;
}

const Frame *buffer_pool_get_frame(BufferPool *pool, uint8_t block_id) {
  if (pool == NULL || block_id == 0 || block_id > pool->num_frames) {
    return NULL;
  }

  int8_t frame_idx = buffer_pool_get_frame_idx(pool, block_id);
  if (frame_idx == -1) {
    frame_idx = buffer_pool_swap_in_block(pool, block_id);
    if (frame_idx == -1) {
      return NULL;
    }
  }

  return &pool->frames[frame_idx];
}

int8_t buffer_pool_swap_in_block(BufferPool *pool, uint8_t block_id) {
  if (pool == NULL || block_id == 0 || block_id > pool->num_frames) {
    return -1;
  }

  int8_t free_frame_idx = buffer_pool_find_free_frame_idx(pool);
  // Evict a frame if no empty frames are found
  if (free_frame_idx == -1) {
    free_frame_idx = buffer_pool_evict_frame(pool);
    if (free_frame_idx == -1) {
      fprintf(stderr,
              "The corresponding block #%d cannot be accessed from disk "
              "because the memory buffers are full.\n",
              block_id);
      return -1;
    }
  }

  // TODO: fseek();

  return free_frame_idx;
}

int8_t buffer_pool_find_free_frame_idx(const BufferPool *pool) {
  if (pool == NULL) {
    return -1;
  }

  for (uint8_t i = 0; i < pool->num_frames; i++) {
    if (frame_get_block_id(&pool->frames[i]) == -1) {
      return i;
    }
  }

  return -1;
}

int8_t buffer_pool_evict_frame(BufferPool *pool) {}
