#include "buffer_manager.h"
#include "storage_manager.h"
#include <stdio.h>

/**
 * Get the block index containing the specified `block_id` in the buffer pool.
 *
 * `block_id` is 1-indexed. Returns -1 if not found.
 */
static int8_t buffer_manager_get_block_idx(const BufferManager *bm,
                                           uint8_t block_id) {
  if (bm == NULL || block_id == 0) {
    return -1;
  }

  for (uint8_t i = 0; i < bm->num_blocks; i++) {
    if (bm->blocks[i].id == block_id) {
      return i;
    }
  }

  return -1;
}

/**
 * Find the index of empty block in the buffer pool.
 *
 * Returns -1 if no empty disk blocks are found.
 */
static int8_t buffer_manager_find_free_block_idx(const BufferManager *bm) {
  if (bm == NULL) {
    return -1;
  }

  for (uint8_t i = 0; i < bm->num_blocks; i++) {
    if (bm->blocks[i].id == -1) {
      return i;
    }
  }

  return -1;
}

/**
 * Evict an unpinned block from the buffer pool, effectively freeing it. Write
 * it to disk if dirty.
 */
static int8_t buffer_manager_evict_block(BufferManager *bm) {
  if (bm == NULL) {
    return -1;
  }

  uint8_t start_idx = (bm->last_evicted_idx + 1) % bm->num_blocks;

  for (uint8_t i = 0; i < bm->num_blocks; i++) {
    uint8_t idx = (start_idx + i) % bm->num_blocks;
    Block *block = &bm->blocks[idx];
    int8_t block_id = block->id;

    if (block_id == -1) {
      fprintf(stderr,
              "Error: Block %d is still empty. No reasons to evict any "
              "disk blocks.\n",
              idx + 1);
      return -1;
    }

    if (block->pinned) {
      continue;
    }

    // Write dirty block to disk
    if (block->dirty) {
      storage_manager_write_block(block_id, block->content);
    }

    // Reset block metadata
    block->id = -1;
    block->dirty = false;
    block->pinned = false;

    bm->last_evicted_idx = idx;
    return idx;
  }

  return -1;
}

/**
 * Swap in requested block on disk into an empty block in the buffer pool.
 * If there are no empty disk blocks, a block is evicted.
 *
 * `block_id` is 1-indexed.
 */
static int8_t buffer_manager_swap_in_block(BufferManager *bm,
                                           uint8_t block_id) {
  if (bm == NULL || block_id == 0) {
    return -1;
  }

  int8_t free_block_idx = buffer_manager_find_free_block_idx(bm);
  // evict a block if no empty disk blocks are found
  if (free_block_idx == -1) {
    free_block_idx = buffer_manager_evict_block(bm);
    if (free_block_idx == -1) {
      return -1;
    }
  }

  Block *new_block = &bm->blocks[free_block_idx];
  storage_manager_read_block(block_id, new_block->content);

  // update block metadata
  new_block->id = block_id;
  new_block->dirty = false;
  new_block->pinned = false;
  return free_block_idx;
}

void buffer_manager_init(BufferManager *bm) {
  if (bm == NULL) {
    return;
  }

  // Last page so we can start from the beginning when we start
  bm->last_evicted_idx = DEFAULT_BUFFER_POOL_SIZE - 1;
  bm->num_blocks = DEFAULT_BUFFER_POOL_SIZE;
  for (uint8_t i = 0; i < bm->num_blocks; i++) {
    block_init(&bm->blocks[i]);
  }
}

/**
 * Get the block containing the specified `block_id` from the buffer pool.
 *
 * `block_id` is 1-indexed.
 */
const Block *buffer_manager_get_block(BufferManager *bm, uint8_t block_id) {
  if (bm == NULL || block_id == 0) {
    return NULL;
  }

  int8_t block_idx = buffer_manager_get_block_idx(bm, block_id);
  if (block_idx == -1) {
    block_idx = buffer_manager_swap_in_block(bm, block_id);
    if (block_idx == -1) {
      fprintf(stderr,
              "The corresponding block #%d cannot be accessed from disk "
              "because the memory buffers are full\n",
              block_id);
      return NULL;
    }
  }
  return &bm->blocks[block_idx];
}

int buffer_manager_pin_block(BufferManager *bm, uint8_t block_id) {
  if (bm == NULL || block_id == 0) {
    return -1;
  }

  int8_t block_idx = buffer_manager_get_block_idx(bm, block_id);
  if (block_idx == -1) {
    block_idx = buffer_manager_swap_in_block(bm, block_id);
    if (block_idx == -1) {
      fprintf(stderr,
              "The corresponding block #%d cannot be pinned because the memory "
              "buffers are full\n",
              block_id);
      return -1;
    }
  }

  Block *block = &bm->blocks[block_idx];
  if (block->pinned) {
    return 0;
  }

  block->pinned = true;
  return 0;
}

int buffer_manager_unpin_block(BufferManager *bm, uint8_t block_id) {
  if (bm == NULL || block_id == 0) {
    return -1;
  }

  int8_t block_idx = buffer_manager_get_block_idx(bm, block_id);
  if (block_idx == -1) {
    fprintf(stderr,
            "The corresponding block %d cannot be unpinned because it "
            "is not in memory.\n",
            block_id);
    return -1;
  }

  Block *block = &bm->blocks[block_idx];
  if (!block->pinned) {
    return 0;
  }

  block->pinned = false;
  return 0;
}
