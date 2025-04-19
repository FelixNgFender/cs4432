#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/**
 * Contains data for one disk block.
 */
typedef struct Block {
  unsigned char content[BLOCK_SIZE];
  // 1-indexed. -1 indicates a free block.
  int8_t id;
  bool dirty;
  bool pinned;
} Block;

void block_init(Block *block) {
  if (block == NULL) {
    return;
  }

  memset(block->content, 0, BLOCK_SIZE);
  block->id = -1;
  block->dirty = false;
  block->pinned = false;
}

void block_set_content(Block *block, size_t offset,
                       const unsigned char *content, size_t size) {
  if (block == NULL || content == NULL || offset >= BLOCK_SIZE ||
      size > BLOCK_SIZE - offset) {
    return;
  }

  block->dirty = true;
  memcpy(block->content + offset, content, size);
}
