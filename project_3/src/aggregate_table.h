#ifndef AGGREGATE_TABLE_H
#define AGGREGATE_TABLE_H

#include "config.h"
#include <stdint.h>
#include <stdlib.h>

typedef enum AggregateFn {
  AGGREGATE_FN_UNKNOWN,
  AGGREGATE_FN_SUM,
  AGGREGATE_FN_AVERAGE,
} AggregateFn;

typedef struct AggregateTableEntry {
  uint64_t sum;
  size_t count;
} AggregateTableEntry;

typedef struct AggregateTable {
  AggregateTableEntry entries[MAX_ADDRESS];
} AggregateTable;

char *aggregate_fn_to_str(AggregateFn fn);
void aggregate_table_init(AggregateTable *table);
void aggregate_table_print(const AggregateTable *table,
                           AggregateFn aggregate_fn);
void aggregate_table_add_entry(AggregateTable *table, uint8_t address,
                               uint16_t random);

#endif // AGGREGATE_TABLE_H
