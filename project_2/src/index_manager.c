#include "index_manager.h"
#include <stdio.h>

void index_manager_init(IndexManager *index) {
  if (index == NULL) {
    return;
  }

  array_index_init(&index->array_index);
  hash_index_init(&index->hash_index);
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
  printf("The hash-based and array-based indexes are built successfully.\n");
}

void index_manager_cleanup(IndexManager *index) {
  if (index == NULL) {
    return;
  }

  array_index_cleanup(&index->array_index);
  hash_index_cleanup(&index->hash_index);
}
