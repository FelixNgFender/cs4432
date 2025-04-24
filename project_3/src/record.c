#include "record.h"
#include "config.h"
#include <stdio.h>

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

int record_parse_from_buffer(const unsigned char *src, Record *record_out) {
  if (record_out == NULL || src == NULL) {
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

  if (prefix != DATA_FILE_PREFIX[0]) {
    fprintf(stderr, "Error: Invalid prefix in record: %s\n", src);
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

int record_parse_from_block(const Block *block, uint8_t record_id,
                            Record *record_out) {
  if (block == NULL || record_id == 0 || record_id > NUM_RECORDS_PER_BLOCK) {
    return -1;
  }

  return record_parse_from_buffer(block->content + RECORD_OFFSET(record_id),
                                  record_out);
}

int record_save_to_block(Block *block, uint8_t record_id,
                         const Record *new_record) {
  if (block == NULL || record_id == 0 || record_id >= NUM_RECORDS_PER_BLOCK) {
    return -1;
  }

  char buffer[RECORD_SIZE + 1];
  int written =
      snprintf(buffer, sizeof(buffer),
               "F%2hhu-Rec%3hhu, Name%3hhu, address%3hhu, %4hu...",
               new_record->location.block_id, new_record->location.record_id,
               new_record->name, new_record->address, new_record->random);

  if (written <= 0 || written >= RECORD_SIZE + 1) {
    return -1;
  }

  block_set_content(block, RECORD_OFFSET(record_id), (unsigned char *)buffer,
                    RECORD_SIZE);
  return 0;
}
