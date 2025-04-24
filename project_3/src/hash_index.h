#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include "config.h"
#include "record.h"
#include <stdlib.h>

// entry for a hash bucket (key + list of locations)
typedef struct HashIndexEntry {
  uint16_t key;
  LocationNode *locations;
  struct HashIndexEntry *next;
} HashIndexEntry;

typedef struct HashIndex {
  HashIndexEntry *buckets[HASH_INDEX_SIZE];
} HashIndex;

void hash_index_print(const HashIndex *index);
void hash_index_init(HashIndex *index);
void hash_index_cleanup(HashIndex *index);
void hash_index_build(HashIndex *index, const Record *records,
                      size_t num_records);
size_t hash_index_get_locations(const HashIndex *index, uint16_t key,
                                RecordLocation *locs_out);

#endif // HASH_INDEX_H
