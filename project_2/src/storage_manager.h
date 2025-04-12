#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <stdint.h>
#include <stdlib.h>

#define DISK_DIR "./Project2Dataset"
#define DATA_FILE_PREFIX 'F'
#define NUM_BLOCKS 99
#define BLOCK_SIZE 4000
#define NUM_RECORDS_PER_BLOCK 100
#define NUM_RECORDS (NUM_RECORDS_PER_BLOCK * NUM_BLOCKS)

#define MIN_BLOCK_ID 1
#define MAX_BLOCK_ID 99
#define MIN_RECORD_ID 1
#define MAX_RECORD_ID 100
#define MIN_NAME 1
#define MAX_NAME 100
#define MIN_ADDRESS 1
#define MAX_ADDRESS 100
#define MIN_RANDOM 1
#define MAX_RANDOM 5000
#define RECORD_SIZE 40
#define RECORD_OFFSET(record_id) ((record_id - 1) * RECORD_SIZE)

/**
 * Read `BLOCK_SIZE` bytes from disk block `block_id` into `dst`.
 */
void storage_manager_read_block(uint8_t block_id, unsigned char *dst);

/**
 * Write `BLOCK_SIZE` bytes from `src` into disk block `block_id`.
 */
void storage_manager_write_block(uint8_t block_id, const unsigned char *src);

typedef struct Record {
  uint8_t block_id;
  uint8_t record_id;
  uint8_t name;
  uint8_t address;
  uint16_t random;
} Record;

void record_print(const Record *record);
size_t records_parse_from_dir(const char *dirname, Record *records_out);
size_t storage_manager_parse_records_from_buffer(const unsigned char *src,
                                                 size_t num_records,
                                                 Record *record_out);

#endif // STORAGE_MANAGER_H
