#include "execution_engine.h"
#include "config.h"
#include "index_manager.h"
#include <stdio.h>

static int execution_engine_create_index(ExecutionEngine *ee) {
  if (ee == NULL) {
    return -1;
  }

  if (ee->index_manager.is_a_index_built) {
    return 0;
  }

  Table table = TABLE_A;
  Record records[NUM_RECORDS];
  if (record_manager_scan_records(&ee->record_manager, table, records, NULL) !=
      NUM_RECORDS) {
    fprintf(
        stderr,
        "Error: Failed to fetch %d records while building index for table %s\n",
        NUM_RECORDS, table_to_str(table));
    return -1;
  }
  index_manager_build(&ee->index_manager, table, records, NUM_RECORDS);

  return 0;
}

static int execution_engine_execute_hash_join(ExecutionEngine *ee) {
  if (ee == NULL) {
    return -1;
  }

  if (execution_engine_create_index(ee) != 0) {
    fprintf(stderr,
            "Error: Failed to create index for table A for hash join\n");
    return -1;
  }

  Record records_b[NUM_RECORDS];
  if (record_manager_scan_records(&ee->record_manager, TABLE_B, records_b,
                                  NULL) != NUM_RECORDS) {
    fprintf(stderr,
            "Error: Failed to fetch %d records from table B for hash join\n",
            NUM_RECORDS);
    return -1;
  }

  printf("A.Col1   A.Col2      B.Col1   B.Col2\n");
  for (size_t i = 0; i < NUM_RECORDS; i++) {
    Record *record_b = &records_b[i];
    Record matched_records_a[RECORD_REPORT_SIZE];
    size_t num_records_found = index_manager_get_records_within_range(
        &ee->index_manager, record_b->random, record_b->random + 1,
        matched_records_a);
    for (size_t j = 0; j < num_records_found; j++) {
      Record *record_a = &matched_records_a[j];
      printf("Name%03hhu, address%03hhu, Name%03hhu, address%03hhu\n",
             record_a->name, record_a->address, record_b->name,
             record_b->address);
    }
  }

  return 0;
}

void execution_engine_init(ExecutionEngine *ee) {
  if (ee == NULL) {
    return;
  }

  index_manager_init(&ee->index_manager);
  record_manager_init(&ee->record_manager);
}

void execution_engine_cleanup(ExecutionEngine *ee) {
  if (ee == NULL) {
    return;
  }

  index_manager_cleanup(&ee->index_manager);
}

int execution_engine_execute_plan(ExecutionEngine *ee, QueryPlan plan) {
  if (ee == NULL) {
    return -1;
  }

  Command command_type = plan.command_type;
  switch (command_type) {
  case COMMAND_HASH_JOIN:
    return execution_engine_execute_hash_join(ee);
  case COMMAND_HASH_AGGREGATION:
    // TODO: implement
    return -1;
  default:
    fprintf(stderr, "Error: Unknown command type.\n");
    return -1;
  }
}
