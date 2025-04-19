#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// each entry in the index holds a linked list of locations
typedef struct ArrayIndexEntry {
  LocationNode *head;
} ArrayIndexEntry;

typedef struct ArrayIndex {
  ArrayIndexEntry entries[ARRAY_INDEX_SIZE];
} ArrayIndex;

void array_index_init(ArrayIndex *index) {
  if (index == NULL) {
    return;
  }

  memset(index, 0, sizeof(ArrayIndex));
}

void array_index_cleanup(ArrayIndex *index) {
  if (index == NULL) {
    return;
  }

  for (size_t i = 0; i < ARRAY_INDEX_SIZE; ++i) {
    LocationNode *curr = index->entries[i].head;
    while (curr != NULL) {
      LocationNode *next = curr->next;
      free(curr);
      curr = next;
    }
    index->entries[i].head = NULL;
  }
}

void array_index_print(const ArrayIndex *index) {
  if (index == NULL) {
    return;
  }

  size_t count = 0;
  printf("Array Index:\n");
  for (size_t i = 0; i < ARRAY_INDEX_SIZE; ++i) {
    LocationNode *curr = index->entries[i].head;
    if (curr != NULL) {
      printf("Key %lu: ", i);
      while (curr != NULL) {
        record_location_print(&curr->location);
        count++;
        curr = curr->next;
      }
    }
  }
  printf("Total locations: %zu\n", count);
}

void array_index_build(ArrayIndex *index, const Record *records,
                       size_t num_records) {
  if (index == NULL || records == NULL) {
    return;
  }

  for (size_t i = 0; i < num_records; ++i) {
    uint16_t key = records[i].random - 1; // Use random as the index key
    if (key >= ARRAY_INDEX_SIZE) {
      continue;
    }

    LocationNode *new_node = check_malloc(sizeof(LocationNode));
    new_node->location = records[i].location;
    new_node->next = index->entries[key].head;
    index->entries[key].head = new_node;
  }
}

size_t array_index_get_locations(const ArrayIndex *index, uint16_t lower,
                                 uint16_t upper, RecordLocation *locs_out) {
  if (index == NULL || lower >= upper || locs_out == NULL) {
    return 0;
  }

  size_t count = 0;
  for (uint16_t i = lower - 1; i < upper - 1; ++i) {
    LocationNode *curr = index->entries[i].head;
    while (curr != NULL) {
      locs_out[count++] = curr->location;
      curr = curr->next;
    }
  }

  return count;
}
