#ifndef RECORD_H
#define RECORD_H

#include "block.h"
#include <stdint.h>

typedef struct RecordLocation {
  uint8_t block_id;
  uint8_t record_id;
} RecordLocation;

// node to store one record location
typedef struct LocationNode {
  RecordLocation location;
  struct LocationNode *next;
} LocationNode;

typedef struct Record {
  RecordLocation location;
  uint16_t random;
  uint8_t name;
  uint8_t address;
} Record;

int record_parse_from_buffer(const unsigned char *src, Record *record_out);
void record_print(const Record *record);
void record_location_print(const RecordLocation *location);
int record_parse_from_buffer(const unsigned char *src, Record *record_out);
/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORDS_PER_BLOCK`.
 */
int record_parse_from_block(const Block *block, uint8_t record_id,
                            Record *record_out);
/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORD
 */
int record_save_to_block(Block *block, uint8_t record_id,
                         const Record *new_record);

#endif // !RECORD_H
