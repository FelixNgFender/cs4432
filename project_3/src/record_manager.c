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

size_t record_manager_scan_records(RecordManager *rm, Table table,
                                   Record *records_out,
                                   size_t *num_blocks_swapped_in) {
  if (rm == NULL || table == TABLE_UNKNOWN || records_out == NULL) {
    return 0;
  }

  size_t count = 0;
  for (uint8_t block_id = MIN_BLOCK_ID; block_id <= MAX_BLOCK_ID; block_id++) {
    bool is_swapped_in = false;
    const Block *block = buffer_manager_get_block(&rm->buffer_manager, table,
                                                  block_id, &is_swapped_in);
    if (block == NULL) {
      continue;
    }
    if (num_blocks_swapped_in != NULL && is_swapped_in) {
      (*num_blocks_swapped_in)++;
    }

    for (uint8_t record_id = MIN_RECORD_ID; record_id <= MAX_RECORD_ID;
         record_id++) {
      Record *record = &records_out[(block_id - 1) * NUM_RECORDS_PER_BLOCK +
                                    (record_id - 1)];
      if (record_parse_from_block(block, table, record_id, record) == 0) {
        count++;
      }
    }
  }
  return count;
}
