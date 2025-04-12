#include "buffer_manager.h"
#include "storage_manager.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define BLOCK_ID(record_number) (record_number / NUM_RECORDS_PER_BLOCK + 1)
#define RECORD_ID(record_number) (record_number % NUM_RECORDS_PER_BLOCK)
#define RECORD_OFFSET(record_id) ((record_id - 1) * RECORD_SIZE)

void block_init(DiskBlock *block);
void block_print(const DiskBlock *block);
/**
 * `block_id` is 1-indexed or -1 (free block).
 */
void block_set_block_id(DiskBlock *block, int8_t block_id);
void block_set_content(DiskBlock *block, const unsigned char *content);
void block_set_dirty(DiskBlock *block, bool dirty);
void block_set_pinned(DiskBlock *block, bool pinned);
int8_t block_get_block_id(const DiskBlock *block);
const unsigned char *block_get_content(const DiskBlock *block);
/**
 * Unsafe getter: Use only when modification is necessary
 */
unsigned char *block_get_content_mutable(DiskBlock *block);
bool block_is_dirty(const DiskBlock *block);
bool block_is_pinned(const DiskBlock *block);
void buffer_manager_print(const BufferManager *bm);
/**
 * Get the block index containing the specified `block_id` in the buffer pool.
 *
 * `block_id` is 1-indexed. Returns -1 if not found.
 */
int8_t buffer_manager_get_block_idx(const BufferManager *pool,
                                    uint8_t block_id);
/**
 * Get the block containing the specified `block_id` from the buffer pool.
 *
 * `block_id` is 1-indexed.
 */
const DiskBlock *buffer_manager_get_block(BufferManager *bm, uint8_t block_id);
/**
 * Get the mutable block containing the specified `block_id` from the buffer
 * pool.
 *
 * `block_id` is 1-indexed.
 */
DiskBlock *buffer_manager_get_block_mutable(BufferManager *bm,
                                            uint8_t block_id);

/**
 * Swap in requested block on disk into an empty block in the buffer pool.
 * If there are no empty disk blocks, a block is evicted.
 *
 * `block_id` is 1-indexed.
 */
int8_t buffer_manager_swap_in_block(BufferManager *bm, uint8_t block_id);
/**
 * Find the index of empty block in the buffer pool.
 *
 * Returns -1 if no empty disk blocks are found.
 */
int8_t buffer_manager_find_free_block_idx(const BufferManager *bm);
/**
 * Evict an unpinned block from the buffer pool, effectively freeing it. Write
 * it to disk if dirty.
 */
int8_t buffer_manager_evict_block(BufferManager *bm);

/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORDS_PER_BLOCK`.
 */
int block_get_record(const DiskBlock *block, uint8_t record_id,
                     Record *record_out);
/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORDS_PER_BLOCK`. Sets
 * the `dirty` flag if the record is modified.
 */
int block_set_record(DiskBlock *block, uint8_t record_id,
                     const unsigned char *new_record);

void buffer_manager_init(BufferManager *bm) {
  if (bm == NULL) {
    return;
  }

  // Last page so we can start from the beginning when we start
  bm->last_evicted_idx = DEFAULT_BUFFER_POOL_SIZE - 1;
  bm->num_disk_blocks = DEFAULT_BUFFER_POOL_SIZE;
  for (size_t i = 0; i < DEFAULT_BUFFER_POOL_SIZE; i++) {
    block_init(&bm->disk_blocks[i]);
  }
}

void buffer_manager_print(const BufferManager *bm) {
  if (bm == NULL) {
    return;
  }

  printf("Buffer pool:\n");
  for (uint8_t i = 0; i < bm->num_disk_blocks; i++) {
    printf("  Block %d: ", i + 1);
    block_print(&bm->disk_blocks[i]);
  }
  printf("\n");
}

int8_t buffer_manager_get_block_idx(const BufferManager *bm, uint8_t block_id) {
  if (bm == NULL || block_id == 0) {
    return -1;
  }

  for (uint8_t i = 0; i < bm->num_disk_blocks; i++) {
    if (block_get_block_id(&bm->disk_blocks[i]) == block_id) {
      return i;
    }
  }

  return -1;
}

const DiskBlock *buffer_manager_get_block(BufferManager *bm, uint8_t block_id) {
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
    printf("Brought File %d from disk; Placed in block %d\n", block_id,
           block_idx + 1);
  } else {
    printf("File %d already in memory; Located in block %d\n", block_id,
           block_idx + 1);
  }

  return &bm->disk_blocks[block_idx];
}

DiskBlock *buffer_manager_get_block_mutable(BufferManager *bm,
                                            uint8_t block_id) {
  return (DiskBlock *)buffer_manager_get_block(bm, block_id);
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

  printf("File %d pinned in block %d; ", block_id, block_idx + 1);
  DiskBlock *block = &bm->disk_blocks[block_idx];
  if (block_is_pinned(block)) {
    printf("Already pinned\n");
    return 0;
  }

  block_set_pinned(block, true);
  printf("Not already pinned\n");
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

  printf("File %d is unpinned in block %d; ", block_id, block_idx + 1);
  DiskBlock *block = &bm->disk_blocks[block_idx];
  if (!block_is_pinned(block)) {
    printf("Block was already unpinned\n");
    return 0;
  }

  block_set_pinned(block, false);
  printf("Block %d was not already unpinned\n", block_idx + 1);
  return 0;
}

int8_t buffer_manager_swap_in_block(BufferManager *bm, uint8_t block_id) {
  if (bm == NULL || block_id == 0) {
    return -1;
  }

  int8_t free_block_idx = buffer_manager_find_free_block_idx(bm);
  // Evict a block if no empty disk blocks are found
  if (free_block_idx == -1) {
    free_block_idx = buffer_manager_evict_block(bm);
    if (free_block_idx == -1) {
      return -1;
    }
  }

  DiskBlock *new_block = &bm->disk_blocks[free_block_idx];
  storage_manager_read_block(block_id, block_get_content_mutable(new_block));

  // Update block metadata
  block_set_block_id(new_block, block_id);
  block_set_dirty(new_block, false);
  block_set_pinned(new_block, false);
  return free_block_idx;
}

int8_t buffer_manager_find_free_block_idx(const BufferManager *bm) {
  if (bm == NULL) {
    return -1;
  }

  for (uint8_t i = 0; i < bm->num_disk_blocks; i++) {
    if (block_get_block_id(&bm->disk_blocks[i]) == -1) {
      return i;
    }
  }

  return -1;
}

int8_t buffer_manager_evict_block(BufferManager *bm) {
  if (bm == NULL) {
    return -1;
  }

  uint8_t start_idx = (bm->last_evicted_idx + 1) % bm->num_disk_blocks;

  for (uint8_t i = 0; i < bm->num_disk_blocks; i++) {
    uint8_t idx = (start_idx + i) % bm->num_disk_blocks;
    DiskBlock *block = &bm->disk_blocks[idx];
    int8_t block_id = block_get_block_id(block);

    if (block_id == -1) {
      fprintf(stderr,
              "Error: Block %d is still empty. No reasons to evict any "
              "disk blocks.\n",
              idx + 1);
      return -1;
    }

    if (block_is_pinned(block)) {
      continue;
    }

    // Write dirty block to disk
    if (block_is_dirty(block)) {
      storage_manager_write_block(block_id, block_get_content(block));
    }

    printf("Evicted file %d from block %d\n", block_id, idx + 1);

    // Reset block metadata
    block_set_block_id(block, -1);
    block_set_dirty(block, false);
    block_set_pinned(block, false);

    bm->last_evicted_idx = idx;
    return idx;
  }

  return -1;
}

int buffer_manager_get_record(BufferManager *bm, const RecordLocation loc,
                              Record *record_out) {
  if (bm == NULL || record_out == NULL) {
    return -1;
  }

  uint8_t block_id = loc.block_id;
  uint8_t record_id = loc.record_id;
  const DiskBlock *block = buffer_manager_get_block(bm, block_id);
  if (block == NULL) {
    return -1;
  }

  return block_get_record(block, record_id, record_out);
}

int instruction_set(BufferManager *bm, uint32_t record_number,
                    const unsigned char *new_record) {
  uint8_t block_id = BLOCK_ID(record_number);
  uint8_t record_id = RECORD_ID(record_number);
  DiskBlock *block = buffer_manager_get_block_mutable(bm, block_id);
  if (block == NULL) {
    fprintf(stderr, "Write was unsuccessful;\n");
    return -1;
  }
  int rc = block_set_record(block, record_id, new_record);
  if (rc == -1) {
    fprintf(stderr, "Write was unsuccessful;\n");
    return -1;
  }
  printf("Write was successful;\n");
  return 0;
}

void block_init(DiskBlock *block) {
  if (block == NULL) {
    return;
  }

  memset(block->content, 0, BLOCK_SIZE);
  block->block_id = -1;
  block->dirty = false;
  block->pinned = false;
}

void block_print(const DiskBlock *block) {
  if (block == NULL) {
    return;
  }

  Record records[NUM_RECORDS_PER_BLOCK] = {0};
  if ((storage_manager_parse_records_from_buffer(
           block->content, NUM_RECORDS_PER_BLOCK, records) !=
       NUM_RECORDS_PER_BLOCK)) {
    fprintf(stderr, "Failed to parse records from block\n");
    return;
  }

  printf("Block ID %d, dirty %d, pinned %d.\n", block->block_id, block->dirty,
         block->pinned);
  if (block_get_block_id(block) != -1) {
    for (uint8_t i = 1; i <= NUM_RECORDS_PER_BLOCK; i++) {
      printf("    Record ID: %d, data: ", i);
      record_print(&records[i - 1]);
    }
  }
  printf("\n");
}

/* Returns 0 if block is NULL, -1 if free */
int8_t block_get_block_id(const DiskBlock *block) {
  if (block == NULL) {
    return 0;
  }

  return block->block_id;
}

const unsigned char *block_get_content(const DiskBlock *block) {
  if (block == NULL) {
    return NULL;
  }

  return block->content;
}

unsigned char *block_get_content_mutable(DiskBlock *block) {
  return (unsigned char *)block_get_content(block);
}

bool block_is_dirty(const DiskBlock *block) {
  if (block == NULL) {
    return false;
  }

  return block->dirty;
}

bool block_is_pinned(const DiskBlock *block) {
  if (block == NULL) {
    return false;
  }

  return block->pinned;
}

void block_set_block_id(DiskBlock *block, int8_t block_id) {
  if (block == NULL || block_id < -1 || block_id == 0) {
    return;
  }

  block->block_id = block_id;
}

void block_set_content(DiskBlock *block, const unsigned char *content) {
  if (block == NULL) {
    return;
  }

  memcpy(block->content, content, BLOCK_SIZE);
}
void block_set_dirty(DiskBlock *block, bool dirty) {
  if (block == NULL) {
    return;
  }

  block->dirty = dirty;
}

void block_set_pinned(DiskBlock *block, bool pinned) {
  if (block == NULL) {
    return;
  }

  block->pinned = pinned;
}

int block_get_record(const DiskBlock *block, uint8_t record_id,
                     Record *record_out) {
  if (block == NULL || record_id == 0 || record_id > NUM_RECORDS_PER_BLOCK) {
    return -1;
  }

  return storage_manager_parse_records_from_buffer(
      block->content + RECORD_OFFSET(record_id), 1, record_out);
}

int block_set_record(DiskBlock *block, uint8_t record_id,
                     const unsigned char *new_record) {
  if (block == NULL || record_id == 0 || record_id >= NUM_RECORDS_PER_BLOCK) {
    return -1;
  }

  block_set_dirty(block, true);
  memcpy(block->content + RECORD_OFFSET(record_id), new_record, RECORD_SIZE);
  return 0;
}
