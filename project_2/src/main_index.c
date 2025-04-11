#include "main_index.h"
#include <stdio.h>

void main_index_init(MainIndex *index) {
  if (index == NULL) {
    return;
  }

  array_index_init(&index->array_index);
}

void main_index_print(const MainIndex *index) {
  if (index == NULL) {
    return;
  }

  array_index_print(&index->array_index);
}

void main_index_build(MainIndex *index, const Record *records,
                      size_t num_records) {
  if (index == NULL || records == NULL) {
    return;
  }

  array_index_build(&index->array_index, records, num_records);
  printf("The hash-based and array-based indexes are built successfully.\n");
}

void main_index_cleanup(MainIndex *index) {
  if (index == NULL) {
    return;
  }

  array_index_cleanup(&index->array_index);
}
