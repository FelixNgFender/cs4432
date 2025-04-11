#ifndef ARRAY_INDEX_H
#define ARRAY_INDEX_H

#include "record.h"

#define ARRAY_INDEX_SIZE MAX_RANDOM
#define INITIAL_CAPACITY 5

typedef struct RecordLocation {
  uint8_t block_id;
  uint8_t record_id;
} RecordLocation;

typedef struct ArrayIndexEntry {
  size_t location_count;
  size_t capacity;
  RecordLocation *locations;
} ArrayIndexEntry;

typedef struct ArrayIndex {
  ArrayIndexEntry entries[ARRAY_INDEX_SIZE];
} ArrayIndex;

void array_index_init(ArrayIndex *index);
void array_index_print(const ArrayIndex *index);
void array_index_build(ArrayIndex *index, const Record *records,
                       size_t num_records);
void array_index_cleanup(ArrayIndex *index);

#endif // ARRAY_INDEX_H
