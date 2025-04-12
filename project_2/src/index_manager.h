#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "array_index.h"
#include "buffer_manager.h"
#include "hash_index.h"

typedef struct IndexManager {
  ArrayIndex array_index;
  HashIndex hash_index;
  BufferManager buffer_manager;
} IndexManager;

void index_manager_init(IndexManager *index);
void index_manager_print(const IndexManager *index);
void index_manager_build(IndexManager *index, const Record *records,
                         size_t num_records);
void index_manager_cleanup(IndexManager *index);
size_t index_manager_get_records_with_key_range(IndexManager *index,
                                                uint16_t key, uint16_t range,
                                                Record *records_out);

#endif // INDEX_MANAGER_H
