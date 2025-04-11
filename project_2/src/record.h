#ifndef RECORD_H
#define RECORD_H

#include <stdint.h>
#include <stdlib.h>

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

typedef struct Record {
  uint8_t block_id;
  uint8_t record_id;
  uint8_t name;
  uint8_t address;
  uint16_t random;
} Record;

size_t records_parse_from_dir(Record *records, const char *dirname);

#endif // RECORD_H
