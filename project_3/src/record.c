#include "record.h"
#include "config.h"
#include <stdio.h>

void record_print(const Record *record) {
  if (record == NULL) {
    return;
  }

  printf("%s%02hhu-Rec%03hhu, Name%03hhu, address%03hhu, %04hu...\n",
         table_to_str(record->table), record->location.block_id,
         record->location.record_id, record->name, record->address,
         record->random);
}

void record_location_print(const RecordLocation *location) {
  if (location == NULL) {
    return;
  }

  printf("Block ID: %02hhu, Record ID: %03hhu\n", location->block_id,
         location->record_id);
}

int record_parse_from_buffer(const unsigned char *src, Table table,
                             Record *record_out) {
  if (record_out == NULL || table == TABLE_UNKNOWN || src == NULL) {
    return -1;
  }

  char prefix;
  if (sscanf((const char *)src,
             "%c%2hhu-Rec%3hhu, Name%3hhu, address%3hhu, %4hu...", &prefix,
             &record_out->location.block_id, &record_out->location.record_id,
             &record_out->name, &record_out->address,
             &record_out->random) != 6) {
    fprintf(stderr, "Error: Failed to parse record: %s\n", src);
    return -1;
  }

  switch (prefix) {
  case 'A':
    record_out->table = TABLE_A;
    break;
  case 'B':
    record_out->table = TABLE_B;
    break;
  default:
    fprintf(stderr, "Error: Invalid table prefix: %c\n", prefix);
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

int record_parse_from_block(const Block *block, Table table, uint8_t record_id,
                            Record *record_out) {
  if (block == NULL || table == TABLE_UNKNOWN || record_id == 0 ||
      record_id > NUM_RECORDS_PER_BLOCK) {
    return -1;
  }

  return record_parse_from_buffer(block->content + RECORD_OFFSET(record_id),
                                  table, record_out);
}
