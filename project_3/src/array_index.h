#ifndef ARRAY_INDEX_H
#define ARRAY_INDEX_H

#include "config.h"
#include "record.h"
#include <stdlib.h>

// each entry in the index holds a linked list of locations
typedef struct ArrayIndexEntry {
  LocationNode *head;
} ArrayIndexEntry;

typedef struct ArrayIndex {
  ArrayIndexEntry entries[ARRAY_INDEX_SIZE];
} ArrayIndex;

void array_index_init(ArrayIndex *index);
void array_index_cleanup(ArrayIndex *index);
void array_index_print(const ArrayIndex *index);
void array_index_build(ArrayIndex *index, const Record *records,
                       size_t num_records);
size_t array_index_get_locations(const ArrayIndex *index, uint16_t lower,
                                 uint16_t upper, RecordLocation *locs_out);

#endif // ARRAY_INDEX_H
