#ifndef BLOCK_H
#define BLOCK_H

#include "config.h"
#include "storage_manager.h"
#include <stdbool.h>
#include <stdlib.h>

/**
 * Contains data for one disk block.
 */
typedef struct Block {
  unsigned char content[BLOCK_SIZE];
  Table table;
  // 1-indexed. -1 indicates a free block.
  int8_t id;
  bool dirty;
  bool pinned;
} Block;

void block_init(Block *block);
void block_set_content(Block *block, size_t offset,
                       const unsigned char *content, size_t size);

#endif // BLOCK_H
