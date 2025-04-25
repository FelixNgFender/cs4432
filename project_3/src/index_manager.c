#include "index_manager.h"
#include <stdio.h>

void index_manager_init(IndexManager *index) {
  if (index == NULL) {
    return;
  }

  index->is_a_index_built = false;
  hash_index_init(&index->hash_index);
}

void index_manager_cleanup(IndexManager *index) {
  if (index == NULL) {
    return;
  }

  hash_index_cleanup(&index->hash_index);
}

void index_manager_print(const IndexManager *index) {
  if (index == NULL) {
    return;
  }

  printf("Index Manager:\n");
  hash_index_print(&index->hash_index);
}

void index_manager_build(IndexManager *index, Table table,
                         const Record *records, size_t num_records) {
  if (index == NULL || table == TABLE_UNKNOWN || records == NULL) {
    return;
  }

  switch (table) {
  case TABLE_A:
    if (index->is_a_index_built) {
      return;
    }
    hash_index_build(&index->hash_index, records, num_records);
    index->is_a_index_built = true;
    break;
  default:
    fprintf(stderr, "Error: Invalid table name.\n");
    break;
  }
  return;
}

size_t index_manager_get_records_within_range(const IndexManager *index,
                                              uint16_t lower, uint16_t upper,
                                              Record *recs_out) {
  if (index == NULL || lower >= upper || recs_out == NULL ||
      !index->is_a_index_built) {
    return 0;
  }

  size_t num_recs = 0;
  if (lower + 1 == upper) {
    num_recs = hash_index_get_records(&index->hash_index, lower, recs_out);
  }
  return num_recs;
}
