#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "block.h"
#include <stdint.h>

typedef struct BufferManager {
  uint8_t num_blocks;
  // index of the last evicted block
  uint8_t last_evicted_idx;
  Block blocks[DEFAULT_BUFFER_POOL_SIZE];
} BufferManager;

void buffer_manager_init(BufferManager *bm);
const Block *buffer_manager_get_block(BufferManager *bm, uint8_t block_id);
int buffer_manager_pin_block(BufferManager *bm, uint8_t block_id);
int buffer_manager_unpin_block(BufferManager *bm, uint8_t block_id);

#endif // BUFFER_MANAGER_H
