#include "config.h"
#include <stdint.h>
#include <stdio.h>

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

int record_parse_from_buffer(const unsigned char *src, Record *record_out) {
  if (record_out == NULL || src == NULL) {
    return -1;
  }

  if (sscanf((const char *)src,
             "F%2hhu-Rec%3hhu, Name%3hhu, address%3hhu, %4hu...",
             &record_out->location.block_id, &record_out->location.record_id,
             &record_out->name, &record_out->address,
             &record_out->random) != 5) {
    fprintf(stderr, "Error: Failed to parse record: %s\n", src);
    return -1;
  }

  if (record_out->location.block_id < MIN_BLOCK_ID ||
      record_out->location.block_id > MAX_BLOCK_ID ||
      record_out->location.record_id < MIN_RECORD_ID ||
      record_out->location.record_id > MAX_RECORD_ID ||
      record_out->name < MIN_NAME || record_out->name > MAX_NAME ||
      record_out->address < MIN_ADDRESS || record_out->address > MAX_ADDRESS ||
      record_out->random < MIN_RANDOM || record_out->random > MAX_RANDOM) {
    fprintf(stderr, "Error: Invalid record values: %s\n", src);
    return -1;
  }

  return 0;
}

void record_print(const Record *record) {
  if (record == NULL) {
    return;
  }

  printf("F%02hhu-Rec%03hhu, Name%03hhu, address%03hhu, %04hu...\n",
         record->location.block_id, record->location.record_id, record->name,
         record->address, record->random);
}

void record_location_print(const RecordLocation *location) {
  if (location == NULL) {
    return;
  }

  printf("Block ID: %02hhu, Record ID: %03hhu\n", location->block_id,
         location->record_id);
}
