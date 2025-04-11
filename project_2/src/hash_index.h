#ifndef HASH_INDEX_H
#define HASH_INDEX_H

#include "index_entry.h"
#include "record.h"

#define HASH_INDEX_SIZE 2048

typedef struct HashNode {
  uint16_t key;
  IndexEntry entry;
  struct HashNode *next;
} HashNode;

typedef struct HashIndex {
  HashNode *buckets[HASH_INDEX_SIZE];
} HashIndex;

void hash_index_init(HashIndex *index);
void hash_index_print(const HashIndex *index);
void hash_index_build(HashIndex *index, const Record *records,
                      size_t num_records);
const IndexEntry *hash_index_get(const HashIndex *index, uint16_t key);
void hash_index_cleanup(HashIndex *index);

#endif // HASH_INDEX_H
