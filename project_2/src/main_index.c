#include "main_index.h"
#include <stdio.h>

void main_index_init(MainIndex *index) {
  if (index == NULL) {
    return;
  }

  array_index_init(&index->array_index);
  hash_index_init(&index->hash_index);
}

void main_index_print(const MainIndex *index) {
  if (index == NULL) {
    return;
  }

  printf("Main Index:\n");
  array_index_print(&index->array_index);
  hash_index_print(&index->hash_index);
}

void main_index_build(MainIndex *index, const Record *records,
                      size_t num_records) {
  if (index == NULL || records == NULL) {
    return;
  }

  array_index_build(&index->array_index, records, num_records);
  hash_index_build(&index->hash_index, records, num_records);
  printf("The hash-based and array-based indexes are built successfully.\n");
}

void main_index_cleanup(MainIndex *index) {
  if (index == NULL) {
    return;
  }

  array_index_cleanup(&index->array_index);
  hash_index_cleanup(&index->hash_index);
}
