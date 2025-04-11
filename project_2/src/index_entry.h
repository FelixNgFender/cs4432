#ifndef INDEX_ENTRY_H
#define INDEX_ENTRY_H

#include <stdint.h>
#include <stdlib.h>

#define INDEX_ENTRY_INITIAL_CAPACITY 5

typedef struct RecordLocation {
  uint8_t block_id;
  uint8_t record_id;
} RecordLocation;

typedef struct IndexEntry {
  size_t location_count;
  size_t capacity;
  RecordLocation *locations;
} IndexEntry;

void index_entry_init(IndexEntry *entry);
void index_entry_print(const IndexEntry *entry);
void index_entry_add(IndexEntry *entry, RecordLocation loc);
void index_entry_cleanup(IndexEntry *entry);

#endif // INDEX_ENTRY_H
