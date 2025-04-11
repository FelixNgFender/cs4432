#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "array_index.h"
#include "hash_index.h"

typedef struct IndexManager {
  ArrayIndex array_index;
  HashIndex hash_index;
} IndexManager;

void index_manager_init(IndexManager *index);
void index_manager_print(const IndexManager *index);
void index_manager_build(IndexManager *index, const Record *records,
                         size_t num_records);
void index_manager_cleanup(IndexManager *index);

#endif // INDEX_MANAGER_H
