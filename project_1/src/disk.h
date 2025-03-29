#ifndef DISK_H
#define DISK_H

#include <stdint.h>

/**
 * Read `BLOCK_SIZE` bytes from disk block `block_id` into `dst`.
 */
void disk_read_block(uint8_t block_id, unsigned char *dst);

/**
 * Write `BLOCK_SIZE` bytes from `src` into disk block `block_id`.
 */
void disk_write_block(uint8_t block_id, const unsigned char *src);

#endif // DISK_H
