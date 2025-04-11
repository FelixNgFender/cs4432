#include "hash_index.h"
#include "mem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void hash_index_add(HashIndex *index, uint16_t key, RecordLocation value);

static size_t hash_uint16(uint16_t key) { return key % HASH_INDEX_SIZE; }

void hash_index_init(HashIndex *index) {
  if (index == NULL) {
    return;
  }

  for (size_t i = 0; i < HASH_INDEX_SIZE; i++) {
    index_entry_init(&index->buckets[i]->entry);
  }
}

void hash_index_print(const HashIndex *index) {
  if (index == NULL) {
    return;
  }

  size_t count = 0;
  for (size_t i = 0; i < HASH_INDEX_SIZE; i++) {
    const HashNode *node = index->buckets[i];
    count += node ? node->entry.location_count : 0;
    while (node) {
      printf("Key %zu: ", i + 1);
      index_entry_print(&node->entry);
      node = node->next;
    }
  }
  printf("Total locations: %zu\n", count);
}

void hash_index_build(HashIndex *index, const Record *records,
                      size_t num_records) {
  if (index == NULL || records == NULL) {
    return;
  }

  for (size_t i = 0; i < num_records; i++) {
    hash_index_add(index, records[i].random,
                   (RecordLocation){.block_id = records[i].block_id,
                                    .record_id = records[i].record_id});
  }
}

void hash_index_add(HashIndex *index, uint16_t key, RecordLocation value) {
  if (index == NULL) {
    return;
  }

  size_t hash = hash_uint16(key);
  HashNode *node = index->buckets[hash];

  // Search for existing node with the same key
  while (node != NULL) {
    if (node->key == key) {
      index_entry_add(&node->entry, value);
      return;
    }
    node = node->next;
  }

  // Key not found: allocate new node
  HashNode *new_node = check_malloc(sizeof(HashNode));

  new_node->key = key;
  new_node->next = index->buckets[hash];
  index->buckets[hash] = new_node;

  index_entry_init(&new_node->entry);
  index_entry_add(&new_node->entry, value);
}

const IndexEntry *hash_index_get(const HashIndex *index, uint16_t key) {
  if (index == NULL) {
    return NULL;
  }

  size_t hash = hash_uint16(key);
  HashNode *node = index->buckets[hash];

  while (node != NULL) {
    if (node->key == key) {
      return &node->entry;
    }
    node = node->next;
  }

  return NULL;
}

void hash_index_cleanup(HashIndex *index) {
  if (index == NULL) {
    return;
  }

  for (size_t i = 0; i < HASH_INDEX_SIZE; i++) {
    HashNode *node = index->buckets[i];
    while (node) {
      HashNode *next = node->next;
      index_entry_cleanup(&node->entry);
      free(node);
      node = next;
    }
  }
}
