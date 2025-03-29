#include "buffer_pool.h"
#include "disk.h"
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
  // Last page so we can start from the beginning
  // when we start
  pool->last_evicted_idx = num_frames - 1;
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
    printf("  Frame %d: ", i + 1);
    print_frame(&pool->frames[i]);
  }
  printf("\n");
}

int8_t buffer_pool_get_frame_idx(const BufferPool *pool, uint8_t block_id) {
  if (pool == NULL || block_id == 0) {
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
  if (pool == NULL || block_id == 0) {
    return NULL;
  }

  int8_t frame_idx = buffer_pool_get_frame_idx(pool, block_id);
  if (frame_idx == -1) {
    frame_idx = buffer_pool_swap_in_block(pool, block_id);
    if (frame_idx == -1) {
      fprintf(stderr,
              "The corresponding block #%d cannot be accessed from disk "
              "because the memory buffers are full\n",
              block_id);
      return NULL;
    }
    printf("Brought File %d from disk; Placed in Frame %d\n", block_id,
           frame_idx + 1);
  } else {
    printf("File %d already in memory; Located in Frame %d\n", block_id,
           frame_idx + 1);
  }

  return &pool->frames[frame_idx];
}

Frame *buffer_pool_get_frame_mutable(BufferPool *pool, uint8_t block_id) {
  return (Frame *)buffer_pool_get_frame(pool, block_id);
}

int buffer_pool_pin_block(BufferPool *pool, uint8_t block_id) {
  if (pool == NULL || block_id == 0) {
    return -1;
  }

  int8_t frame_idx = buffer_pool_get_frame_idx(pool, block_id);
  if (frame_idx == -1) {
    frame_idx = buffer_pool_swap_in_block(pool, block_id);
    if (frame_idx == -1) {
      fprintf(stderr,
              "The corresponding block #%d cannot be pinned because the memory "
              "buffers are full\n",
              block_id);
      return -1;
    }
  }

  printf("File %d pinned in Frame %d; ", block_id, frame_idx + 1);
  Frame *frame = &pool->frames[frame_idx];
  if (frame_is_pinned(frame)) {
    printf("Already pinned\n");
    return 0;
  }

  frame_set_pinned(frame, true);
  printf("Not already pinned\n");
  return 0;
}

int buffer_pool_unpin_block(BufferPool *pool, uint8_t block_id) {
  if (pool == NULL || block_id == 0) {
    return -1;
  }

  int8_t frame_idx = buffer_pool_get_frame_idx(pool, block_id);
  if (frame_idx == -1) {
    fprintf(stderr,
            "The corresponding block %d cannot be unpinned because it "
            "is not in memory.\n",
            block_id);
    return -1;
  }

  printf("File %d is unpinned in frame %d; ", block_id, frame_idx + 1);
  Frame *frame = &pool->frames[frame_idx];
  if (!frame_is_pinned(frame)) {
    printf("Frame was already unpinned\n");
    return 0;
  }

  frame_set_pinned(frame, false);
  printf("Frame %d was not already unpinned\n", frame_idx + 1);
  return 0;
}

int8_t buffer_pool_swap_in_block(BufferPool *pool, uint8_t block_id) {
  if (pool == NULL || block_id == 0) {
    return -1;
  }

  int8_t free_frame_idx = buffer_pool_find_free_frame_idx(pool);
  // Evict a frame if no empty frames are found
  if (free_frame_idx == -1) {
    free_frame_idx = buffer_pool_evict_frame(pool);
    if (free_frame_idx == -1) {
      return -1;
    }
  }

  Frame *new_frame = &pool->frames[free_frame_idx];
  disk_read_block(block_id, frame_get_content_mutable(new_frame));

  // Update frame metadata
  frame_set_block_id(new_frame, block_id);
  frame_set_dirty(new_frame, false);
  frame_set_pinned(new_frame, false);
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

int8_t buffer_pool_evict_frame(BufferPool *pool) {
  if (pool == NULL) {
    return -1;
  }

  uint8_t start_idx = (pool->last_evicted_idx + 1) % pool->num_frames;

  for (uint8_t i = 0; i < pool->num_frames; i++) {
    uint8_t idx = (start_idx + i) % pool->num_frames;
    Frame *frame = &pool->frames[idx];
    int8_t block_id = frame_get_block_id(frame);

    if (block_id == -1) {
      fprintf(
          stderr,
          "Error: Frame %d is still empty. No reasons to evict any frames.\n",
          idx + 1);
      return -1;
    }

    if (frame_is_pinned(frame)) {
      continue;
    }

    // Write dirty frame to disk
    if (frame_is_dirty(frame)) {
      disk_write_block(block_id, frame_get_content(frame));
    }

    printf("Evicted file %d from Frame %d\n", block_id, idx + 1);

    // Reset frame metadata
    frame_set_block_id(frame, -1);
    frame_set_dirty(frame, false);
    frame_set_pinned(frame, false);

    pool->last_evicted_idx = idx;
    return idx;
  }

  return -1;
}
