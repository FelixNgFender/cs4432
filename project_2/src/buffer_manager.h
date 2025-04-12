#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "index_entry.h"
#include "storage_manager.h"
#include <stdbool.h>
#include <stdint.h>

#define DEFAULT_BUFFER_POOL_SIZE 10

/**
 * Contains data for one disk block.
 */
typedef struct DiskBlock {
  unsigned char content[BLOCK_SIZE];
  // 1-indexed. -1 indicates a free block.
  int8_t block_id;
  bool dirty;
  bool pinned;
} DiskBlock;

typedef struct BufferManager {
  uint8_t num_disk_blocks;
  // Index of the last evicted block
  uint8_t last_evicted_idx;
  DiskBlock disk_blocks[DEFAULT_BUFFER_POOL_SIZE];
} BufferManager;

void buffer_manager_init(BufferManager *bm);

int buffer_manager_get_record(BufferManager *bm, const RecordLocation loc,
                              Record *record_out);

#endif
