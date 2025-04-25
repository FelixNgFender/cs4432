#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <stdint.h>

typedef enum Table {
  TABLE_UNKNOWN,
  TABLE_A,
  TABLE_B,
} Table;

char *table_to_str(Table table);
/**
 * Read `BLOCK_SIZE` bytes from disk block `block_id` into `dst`.
 */
void storage_manager_read_block(Table table, uint8_t block_id,
                                unsigned char *dst);
/**
 * Write `BLOCK_SIZE` bytes from `src` into disk block `block_id`.
 */
void storage_manager_write_block(Table table, uint8_t block_id,
                                 const unsigned char *src);

#endif // STORAGE_MANAGER_H
