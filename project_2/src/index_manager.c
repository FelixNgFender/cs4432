#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct IndexManager {
  HashIndex hash_index;
  ArrayIndex array_index;
  bool is_built;
} IndexManager;

void index_manager_init(IndexManager *index) {
  if (index == NULL) {
    return;
  }

  index->is_built = false;
  array_index_init(&index->array_index);
  hash_index_init(&index->hash_index);
}

void index_manager_cleanup(IndexManager *index) {
  if (index == NULL) {
    return;
  }

  array_index_cleanup(&index->array_index);
  hash_index_cleanup(&index->hash_index);
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
  index->is_built = true;
  printf("The hash-based and array-based indexes are built successfully.\n");
}

size_t
index_manager_get_record_locations_within_range(const IndexManager *index,
                                                uint16_t lower, uint16_t upper,
                                                RecordLocation *locs_out) {
  if (index == NULL || lower >= upper || locs_out == NULL || !index->is_built) {
    return 0;
  }

  size_t num_locations = 0;
  if (lower + 1 == upper) {
    num_locations =
        hash_index_get_locations(&index->hash_index, lower, locs_out);
    printf("Used hash-based index.\n");
  } else {
    num_locations =
        array_index_get_locations(&index->array_index, lower, upper, locs_out);
    printf("Used array-based index.\n");
  }
  return num_locations;
}
