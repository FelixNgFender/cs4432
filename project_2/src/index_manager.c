#include "index_manager.h"
#include "array_index.h"
#include "buffer_manager.h"
#include "hash_index.h"
#include <stdio.h>

void index_manager_init(IndexManager *index) {
  if (index == NULL) {
    return;
  }

  index->index_manager_built = false;
  array_index_init(&index->array_index);
  hash_index_init(&index->hash_index);
  buffer_manager_init(&index->buffer_manager);
}

void index_manager_print(const IndexManager *index) {
  if (index == NULL) {
    return;
  }

  printf("Index Manager:\n");
  array_index_print(&index->array_index);
  hash_index_print(&index->hash_index);
}

void index_manager_build(IndexManager *index, const Record *records,
                         size_t num_records) {
  if (index == NULL || records == NULL) {
    return;
  }

  array_index_build(&index->array_index, records, num_records);
  hash_index_build(&index->hash_index, records, num_records);
  index->index_manager_built = true;
  printf("The hash-based and array-based indexes are built successfully.\n");
}

void index_manager_cleanup(IndexManager *index) {
  if (index == NULL) {
    return;
  }

  array_index_cleanup(&index->array_index);
  hash_index_cleanup(&index->hash_index);
}

size_t index_manager_get_records_with_key_range(IndexManager *index,
                                                uint16_t key, uint16_t range,
                                                Record *records_out) {
  if (index == NULL || key < MIN_RANDOM || key > MAX_RANDOM || range == 0 ||
      key + range > MAX_RANDOM) {
    return -1;
  }

  if (!index->index_manager_built) {
    // TODO: implement a table scan
    printf("Used table scan.\n");
    return -1;
  }

  const IndexEntry *entry;
  if (range == 1) {
    entry = hash_index_get(&index->hash_index, key);
    printf("Used hash-based index.\n");
  } else {
    // TODO: adjust, should support range queries
    entry = array_index_get_range(&index->array_index, key, range);
    printf("Used array-based index.\n");
  }

  if (entry == NULL) {
    fprintf(stderr, "No entry found for key %u.\n", key);
    return -1;
  }

  for (size_t i = 0; i < entry->location_count; i++) {
    if ((buffer_manager_get_record(&index->buffer_manager, entry->locations[i],
                                   &records_out[i])) == -1) {
      fprintf(stderr, "Failed to get record for location %u:%u.\n",
              entry->locations[i].block_id, entry->locations[i].record_id);
      return -1;
    }
  }

  return entry->location_count;
}
