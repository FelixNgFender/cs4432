#ifndef ARRAY_INDEX_H
#define ARRAY_INDEX_H

#include "index_entry.h"
#include "record.h"

#define ARRAY_INDEX_SIZE MAX_RANDOM

typedef struct ArrayIndex {
  IndexEntry entries[ARRAY_INDEX_SIZE];
} ArrayIndex;

void array_index_init(ArrayIndex *index);
void array_index_print(const ArrayIndex *index);
void array_index_build(ArrayIndex *index, const Record *records,
                       size_t num_records);
const IndexEntry *array_index_get_range(const ArrayIndex *index, uint16_t start,
                                        uint16_t range);
void array_index_cleanup(ArrayIndex *index);

#endif // ARRAY_INDEX_H
