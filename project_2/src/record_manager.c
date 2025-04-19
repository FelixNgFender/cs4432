#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct RecordManager {
  BufferManager *buffer_manager;
} RecordManager;

/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORDS_PER_BLOCK`.
 */
static int record_parse_from_block(const Block *block, uint8_t record_id,
                                   Record *record_out) {
  if (block == NULL || record_id == 0 || record_id > NUM_RECORDS_PER_BLOCK) {
    return -1;
  }

  return record_parse_from_buffer(block->content + RECORD_OFFSET(record_id),
                                  record_out);
}

/**
 * `record_id` is 1-indexed and cannot exceed `NUM_RECORD
 */
// static int record_save_to_block(Block *block, uint8_t record_id,
//                                 const Record *new_record) {
//   if (block == NULL || record_id == 0 || record_id >= NUM_RECORDS_PER_BLOCK)
//   {
//     return -1;
//   }
//
//   char buffer[RECORD_SIZE + 1];
//   int written =
//       snprintf(buffer, sizeof(buffer),
//                "F%2hhu-Rec%3hhu, Name%3hhu, address%3hhu, %4hu...",
//                new_record->location.block_id, new_record->location.record_id,
//                new_record->name, new_record->address, new_record->random);
//
//   if (written <= 0 || written >= RECORD_SIZE + 1) {
//     return -1;
//   }
//
//   block_set_content(block, RECORD_OFFSET(record_id), (unsigned char *)buffer,
//                     RECORD_SIZE);
//   return 0;
// }

void record_manager_init(RecordManager *rm) {
  if (rm == NULL) {
    return;
  }

  buffer_manager_init(rm->buffer_manager);
}

size_t record_manager_scan_records(RecordManager *rm, Record *records_out) {
  if (rm == NULL || records_out == NULL) {
    return 0;
  }

  size_t count = 0;
  for (uint8_t block_id = MIN_BLOCK_ID; block_id <= MAX_BLOCK_ID; block_id++) {
    const Block *block = buffer_manager_get_block(rm->buffer_manager, block_id);
    if (block == NULL) {
      continue;
    }

    for (uint8_t record_id = MIN_RECORD_ID; record_id <= MAX_RECORD_ID;
         record_id++) {
      Record *record = &records_out[(block_id - 1) * NUM_RECORDS_PER_BLOCK +
                                    (record_id - 1)];
      if (record_parse_from_block(block, record_id, record) == 0) {
        count++;
      }
    }
  }
  return count;
}

size_t record_manager_fetch_records(RecordManager *rm,
                                    const RecordLocation *locs, size_t num_locs,
                                    Record *records_out) {
  if (rm == NULL || locs == NULL || records_out == NULL) {
    return 0;
  }

  size_t count = 0;
  for (size_t i = 0; i < num_locs; ++i) {
    uint8_t block_id = locs[i].block_id;
    uint8_t record_id = locs[i].record_id;
    const Block *block = buffer_manager_get_block(rm->buffer_manager, block_id);
    if (block == NULL) {
      continue;
    }

    Record *record = &records_out[count];
    if (record_parse_from_block(block, record_id, record) == 0) {
      count++;
    }
  }
  return count;
}
