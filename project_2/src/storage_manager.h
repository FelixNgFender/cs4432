#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <stdint.h>

/**
 * Read `BLOCK_SIZE` bytes from disk block `block_id` into `dst`.
 */
void storage_manager_read_block(uint8_t block_id, unsigned char *dst);
/**
 * Write `BLOCK_SIZE` bytes from `src` into disk block `block_id`.
 */
void storage_manager_write_block(uint8_t block_id, const unsigned char *src);

#endif // STORAGE_MANAGER_H
