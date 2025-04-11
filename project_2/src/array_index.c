#include "array_index.h"
#include "mem.h"
#include <stdio.h>

void array_index_entry_init(ArrayIndexEntry *entry);
void array_index_add(ArrayIndex *index, uint16_t key, RecordLocation value);

void array_index_init(ArrayIndex *index) {
  if (index == NULL) {
    return;
  }

  for (size_t i = 0; i < ARRAY_INDEX_SIZE; i++) {
    array_index_entry_init(&index->entries[i]);
  }
}

void array_index_entry_init(ArrayIndexEntry *entry) {
  if (entry == NULL) {
    return;
  }

  entry->location_count = 0;
  entry->capacity = INITIAL_CAPACITY;
  entry->locations = check_malloc(entry->capacity * sizeof(RecordLocation));
}

void array_index_print(const ArrayIndex *index) {
  if (index == NULL) {
    return;
  }
  int count = 0;

  for (size_t i = 0; i < ARRAY_INDEX_SIZE; i++) {
    const ArrayIndexEntry *entry = &index->entries[i];
    printf("Key %zu: ", i + 1);
    if (entry->location_count == 0) {
      printf("(no locations)\n");
      continue;
    }
    for (size_t j = 0; j < entry->location_count; j++) {
      printf("(%u, %u) ", entry->locations[j].block_id,
             entry->locations[j].record_id);
      count++;
    }
    printf("\n");
  }
  printf("Total locations: %d\n", count);
}

void array_index_cleanup(ArrayIndex *index) {
  if (index == NULL) {
    return;
  }

  for (size_t i = 0; i < ARRAY_INDEX_SIZE; i++) {
    ArrayIndexEntry *entry = &index->entries[i];
    if (entry->locations == NULL) {
      continue;
    }
    free(entry->locations);
  }
}

void array_index_build(ArrayIndex *index, const Record *records,
                       size_t num_records) {
  for (size_t i = 0; i < num_records; i++) {
    array_index_add(index, records[i].random,
                    (RecordLocation){.block_id = records[i].block_id,
                                     .record_id = records[i].record_id});
  }
}

void array_index_add(ArrayIndex *index, uint16_t key, RecordLocation value) {
  if (index == NULL) {
    return;
  }

  ArrayIndexEntry *entry = &index->entries[key - 1];
  if (entry->location_count >= entry->capacity) {
    entry->capacity *= 2;
    entry->locations = check_realloc(entry->locations,
                                     entry->capacity * sizeof(RecordLocation));
  }
  entry->locations[entry->location_count] = value;
  entry->location_count++;
}
