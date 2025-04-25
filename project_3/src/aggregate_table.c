#include "aggregate_table.h"
#include <stdio.h>
#include <string.h>

char *aggregate_fn_to_str(AggregateFn fn) {
  switch (fn) {
  case AGGREGATE_FN_SUM:
    return "SUM(RandomV)";
  case AGGREGATE_FN_AVERAGE:
    return "AVG(RandomV)";
  default:
    return "UNKNOWN";
  }
}

void aggregate_table_init(AggregateTable *table) {
  if (table == NULL) {
    return;
  }

  memset(table->entries, 0, sizeof(table->entries));
}

void aggregate_table_print(const AggregateTable *table,
                           AggregateFn aggregate_fn) {
  if (table == NULL || aggregate_fn == AGGREGATE_FN_UNKNOWN) {
    return;
  }

  printf("Col2        %s\n", aggregate_fn_to_str(aggregate_fn));
  switch (aggregate_fn) {
  case AGGREGATE_FN_SUM:
    for (uint8_t addr = 0; addr < MAX_ADDRESS; addr++) {
      if (table->entries[addr].count > 0) {
        printf("address%03hhu, %lu\n", addr + 1, table->entries[addr].sum);
      }
    }
    break;
  case AGGREGATE_FN_AVERAGE:
    for (uint8_t addr = 0; addr < MAX_ADDRESS; addr++) {
      if (table->entries[addr].count > 0) {
        double average =
            (double)table->entries[addr].sum / table->entries[addr].count;
        printf("address%03hhu, %.2f\n", addr + 1, average);
      }
    }
    break;
  default:
    fprintf(stderr, "Error: Unknown aggregate function\n");
    break;
  }
}

void aggregate_table_add_entry(AggregateTable *table, uint8_t address,
                               uint16_t random) {
  if (table == NULL || address < MIN_ADDRESS || address > MAX_ADDRESS ||
      random < MIN_RANDOM || random > MAX_RANDOM) {
    return;
  }

  AggregateTableEntry *entry = &table->entries[address - 1];
  entry->sum += random;
  entry->count++;
}
