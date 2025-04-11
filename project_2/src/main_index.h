#ifndef MAIN_INDEX_H
#define MAIN_INDEX_H

#include "array_index.h"
#include "hash_index.h"

typedef struct MainIndex {
  ArrayIndex array_index;
  HashIndex hash_index;
} MainIndex;

void main_index_init(MainIndex *index);
void main_index_print(const MainIndex *index);
void main_index_build(MainIndex *index, const Record *records,
                      size_t num_records);
void main_index_cleanup(MainIndex *index);

#endif // MAIN_INDEX_H
