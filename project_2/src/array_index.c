#include "array_index.h"
#include <stdio.h>

void array_index_init(ArrayIndex *index) {
  if (index == NULL) {
    return;
  }

  for (size_t i = 0; i < ARRAY_INDEX_SIZE; i++) {
    index_entry_init(&index->entries[i]);
  }
}

void array_index_print(const ArrayIndex *index) {
  if (index == NULL) {
    return;
  }

  printf("Array Index:\n");
  size_t count = 0;
  for (size_t i = 0; i < ARRAY_INDEX_SIZE; i++) {
    const IndexEntry *entry = &index->entries[i];
    count += entry->location_count;
    printf("Key %zu: ", i + 1);
    index_entry_print(entry);
  }
  printf("Total locations: %zu\n", count);
}

void array_index_build(ArrayIndex *index, const Record *records,
                       size_t num_records) {
  if (index == NULL || records == NULL) {
    return;
  }

  for (size_t i = 0; i < num_records; i++) {
    index_entry_add(&index->entries[records[i].random - 1],
                    (RecordLocation){.block_id = records[i].block_id,
                                     .record_id = records[i].record_id});
  }
}

const IndexEntry *array_index_get_range(const ArrayIndex *index, uint16_t start,
                                        uint16_t range) {
  if (index == NULL || start < 1 || range < 1 ||
      start + range - 1 > ARRAY_INDEX_SIZE) {
    return NULL;
  }

  return &index->entries[start - 1];
}

void array_index_cleanup(ArrayIndex *index) {
  if (index == NULL) {
    return;
  }

  for (size_t i = 0; i < ARRAY_INDEX_SIZE; i++) {
    IndexEntry *entry = &index->entries[i];
    index_entry_cleanup(entry);
  }
}
