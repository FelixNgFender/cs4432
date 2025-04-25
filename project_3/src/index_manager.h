#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include "hash_index.h"
#include <stdbool.h>

typedef struct IndexManager {
  HashIndex hash_index;
  bool is_a_index_built;
} IndexManager;

void index_manager_init(IndexManager *index);
void index_manager_cleanup(IndexManager *index);
void index_manager_print(const IndexManager *index);
void index_manager_build(IndexManager *index, Table table,
                         const Record *records, size_t num_records);
size_t index_manager_get_records_within_range(const IndexManager *index,
                                              uint16_t lower, uint16_t upper,
                                              Record *recs_out);

#endif // INDEX_MANAGER_H
