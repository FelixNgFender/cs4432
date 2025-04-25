#ifndef RECORD_MANAGER_H
#define RECORD_MANAGER_H

#include "buffer_manager.h"
#include "record.h"

typedef struct RecordManager {
  BufferManager buffer_manager;
} RecordManager;

void record_manager_init(RecordManager *rm);
size_t record_manager_scan_records(RecordManager *rm, Table table,
                                   Record *records_out,
                                   size_t *num_blocks_swapped_in);

#endif // RECORD_MANAGER_H
