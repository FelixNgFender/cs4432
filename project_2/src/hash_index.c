#include "config.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// entry for a hash bucket (key + list of locations)
typedef struct HashIndexEntry {
  uint16_t key;
  LocationNode *locations;
  struct HashIndexEntry *next;
} HashIndexEntry;

typedef struct HashIndex {
  HashIndexEntry *buckets[HASH_INDEX_SIZE];
} HashIndex;

static inline size_t hash_key(uint16_t key) { return key % HASH_INDEX_SIZE; }

void hash_index_init(HashIndex *index) {
  if (index == NULL) {
    return;
  }

  memset(index, 0, sizeof(HashIndex));
}

void hash_index_cleanup(HashIndex *index) {
  if (index == NULL) {
    return;
  }

  for (size_t i = 0; i < HASH_INDEX_SIZE; ++i) {
    HashIndexEntry *entry = index->buckets[i];
    while (entry != NULL) {
      HashIndexEntry *next_entry = entry->next;

      // free the list of locations
      LocationNode *loc = entry->locations;
      while (loc != NULL) {
        LocationNode *next_loc = loc->next;
        free(loc);
        loc = next_loc;
      }

      free(entry);
      entry = next_entry;
    }
    index->buckets[i] = NULL;
  }
}

void hash_index_build(HashIndex *index, const Record *records,
                      size_t num_records) {
  if (index == NULL || records == NULL) {
    return;
  }

  for (size_t i = 0; i < num_records; ++i) {
    uint16_t key = records[i].random;
    size_t h = hash_key(key);

    HashIndexEntry *entry = index->buckets[h];
    // search for existing key
    while (entry != NULL && entry->key != key) {
      entry = entry->next;
    }

    // if key not found, create a new entry
    if (entry == NULL) {
      entry = check_malloc(sizeof(HashIndexEntry));
      entry->key = key;
      entry->locations = NULL;
      entry->next = index->buckets[h];
      index->buckets[h] = entry;
    }

    // add location to the front of the list
    LocationNode *new_loc = check_malloc(sizeof(LocationNode));
    new_loc->location = records[i].location;
    new_loc->next = entry->locations;
    entry->locations = new_loc;
  }
}

void hash_index_print(const HashIndex *index) {
  if (index == NULL) {
    return;
  }

  printf("Hash Index:\n");
  size_t count = 0;
  for (size_t i = 0; i < HASH_INDEX_SIZE; ++i) {
    HashIndexEntry *entry = index->buckets[i];
    while (entry != NULL) {
      printf("Key %u: ", entry->key);
      LocationNode *loc = entry->locations;
      while (loc != NULL) {
        record_location_print(&loc->location);
        loc = loc->next;
        count++;
      }
      entry = entry->next;
    }
  }
  printf("Total locations: %zu\n", count);
}

size_t hash_index_get_locations(const HashIndex *index, uint16_t key,
                                RecordLocation *locs_out) {
  if (index == NULL || locs_out == NULL) {
    return 0;
  }

  size_t h = hash_key(key);
  HashIndexEntry *entry = index->buckets[h];

  // find the matching key
  while (entry != NULL && entry->key != key) {
    entry = entry->next;
  }

  size_t count = 0;
  if (entry != NULL) {
    LocationNode *loc = entry->locations;
    while (loc != NULL) {
      locs_out[count++] = loc->location;
      loc = loc->next;
    }
  }

  return count;
}
