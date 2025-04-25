#ifndef RECORD_H
#define RECORD_H

#include "block.h"
#include <stdint.h>

typedef struct RecordLocation {
  uint8_t block_id;
  uint8_t record_id;
} RecordLocation;

typedef struct Record {
  Table table;
  RecordLocation location;
  uint16_t random;
  uint8_t name;
  uint8_t address;
} Record;

// node to store one record
typedef struct RecordNode {
  Record record;
  struct RecordNode *next;
} RecordNode;

void record_print(const Record *record);
void record_location_print(const RecordLocation *location);
int record_parse_from_buffer(const unsigned char *src, Table table,
                             Record *record_out);
/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORDS_PER_BLOCK`.
 */
int record_parse_from_block(const Block *block, Table table, uint8_t record_id,
                            Record *record_out);

#endif // !RECORD_H
