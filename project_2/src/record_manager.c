#include "record_manager.h"
#include "block.h"
#include "record.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void record_manager_init(RecordManager *rm) {
  if (rm == NULL) {
    return;
  }

  buffer_manager_init(&rm->buffer_manager);
}

size_t record_manager_scan_records(RecordManager *rm, Record *records_out) {
  if (rm == NULL || records_out == NULL) {
    return 0;
  }

  size_t count = 0;
  for (uint8_t block_id = MIN_BLOCK_ID; block_id <= MAX_BLOCK_ID; block_id++) {
    const Block *block =
        buffer_manager_get_block(&rm->buffer_manager, block_id);
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
    const Block *block =
        buffer_manager_get_block(&rm->buffer_manager, block_id);
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
