#include "index_entry.h"
#include "mem.h"
#include <stdio.h>

void index_entry_init(IndexEntry *entry) {
  if (entry == NULL) {
    return;
  }

  entry->location_count = 0;
  entry->capacity = INDEX_ENTRY_INITIAL_CAPACITY;
  entry->locations = check_malloc(entry->capacity * sizeof(RecordLocation));
}

void index_entry_print(const IndexEntry *entry) {
  if (entry == NULL) {
    return;
  }

  if (entry->location_count == 0) {
    printf("(no locations)\n");
    return;
  }
  for (size_t j = 0; j < entry->location_count; j++) {
    printf("(%u, %u) ", entry->locations[j].block_id,
           entry->locations[j].record_id);
  }
  printf("\n");
}

void index_entry_add(IndexEntry *entry, RecordLocation loc) {
  if (entry == NULL) {
    return;
  }

  if (entry->location_count >= entry->capacity) {
    entry->capacity *= 2;
    entry->locations = check_realloc(entry->locations,
                                     entry->capacity * sizeof(RecordLocation));
  }
  entry->locations[entry->location_count] = loc;
  entry->location_count++;
}

void index_entry_cleanup(IndexEntry *entry) {
  if (entry == NULL) {
    return;
  }

  free(entry->locations);
}
