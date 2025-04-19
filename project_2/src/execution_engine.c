#include "execution_engine.h"
#include <stdio.h>

static int execution_engine_create_index(ExecutionEngine *ee) {
  if (ee == NULL) {
    return -1;
  }

  if (ee->index_manager.is_built) {
    fprintf(stderr, "Error: Index already built.\n");
    return -1;
  }

  Record records[NUM_RECORDS];
  if (record_manager_scan_records(&ee->record_manager, records) !=
      NUM_RECORDS) {
    fprintf(stderr, "Error: Failed to fetch %d records while building index\n",
            NUM_RECORDS);
    return -1;
  }
  index_manager_build(&ee->index_manager, records, NUM_RECORDS);

  return 0;
}

static int execution_engine_execute_equality_query(ExecutionEngine *ee,
                                                   uint16_t v1) {
  if (ee == NULL) {
    return -1;
  }

  const char *query_type = NULL;
  Record records_to_report[RECORD_REPORT_SIZE];
  size_t num_records_found;
  if (ee->index_manager.is_built) {
    RecordLocation locs[RECORD_REPORT_SIZE];
    size_t num_locs = index_manager_get_record_locations_within_range(
        &ee->index_manager, v1, v1 + 1, locs);
    if (num_locs > RECORD_REPORT_SIZE) {
      printf(
          "Warning: More than %d (%zu) records found, only showing first %d\n",
          RECORD_REPORT_SIZE, num_locs, RECORD_REPORT_SIZE);
      num_locs = RECORD_REPORT_SIZE;
    }
    num_records_found = record_manager_fetch_records(
        &ee->record_manager, locs, num_locs, records_to_report);
    query_type = "Array-based index";
  } else {
    Record records[NUM_RECORDS];
    if (record_manager_scan_records(&ee->record_manager, records) !=
        NUM_RECORDS) {
      fprintf(stderr, "Error: Failed to table scan while executing query\n");
      return -1;
    }

    num_records_found = 0;
    for (size_t i = 0; i < NUM_RECORDS; i++) {
      if (records[i].random == v1) {
        records_to_report[num_records_found++] = records[i];
        if (num_records_found >= RECORD_REPORT_SIZE) {
          printf(
              "Warning: More than %d (%zu) records found, only showing first "
              "%d\n",
              RECORD_REPORT_SIZE, num_records_found, RECORD_REPORT_SIZE);
          break;
        }
      }
    }
    query_type = "Table Scan";
  }

  for (size_t i = 0; i < num_records_found; i++) {
    record_print(&records_to_report[i]);
  }
  printf("Index type used: %s\n", query_type);
  return 0;
}

static int execution_engine_execute_range_query(ExecutionEngine *ee,
                                                uint16_t v1, uint16_t v2) {
  if (ee == NULL) {
    return -1;
  }

  const char *query_type = NULL;
  Record records_to_report[RECORD_REPORT_SIZE];
  size_t num_records_found;
  if (ee->index_manager.is_built) {
    RecordLocation locs[RECORD_REPORT_SIZE];
    size_t num_locs = index_manager_get_record_locations_within_range(
        &ee->index_manager, v1 + 1, v2, locs);
    if (num_locs > RECORD_REPORT_SIZE) {
      printf(
          "Warning: More than %d (%zu) records found, only showing first %d\n",
          RECORD_REPORT_SIZE, num_locs, RECORD_REPORT_SIZE);
      num_locs = RECORD_REPORT_SIZE;
    }
    num_records_found = record_manager_fetch_records(
        &ee->record_manager, locs, num_locs, records_to_report);
    query_type = "Hash-based index";
  } else {
    Record records[NUM_RECORDS];
    if (record_manager_scan_records(&ee->record_manager, records) !=
        NUM_RECORDS) {
      fprintf(stderr, "Error: Failed to table scan while executing query\n");
      return -1;
    }

    num_records_found = 0;
    for (size_t i = 0; i < NUM_RECORDS; i++) {
      if (records[i].random > v1 && records[i].random < v2) {
        records_to_report[num_records_found++] = records[i];
        if (num_records_found >= RECORD_REPORT_SIZE) {
          printf(
              "Warning: More than %d (%zu) records found, only showing first "
              "%d\n",
              RECORD_REPORT_SIZE, num_records_found, RECORD_REPORT_SIZE);
          break;
        }
      }
    }
    query_type = "Table Scan";
  }

  for (size_t i = 0; i < num_records_found; i++) {
    record_print(&records_to_report[i]);
  }
  printf("Index type used: %s\n", query_type);
  return 0;
}

static int execution_engine_execute_inequality_query(ExecutionEngine *ee,
                                                     uint16_t v1) {
  if (ee == NULL) {
    return -1;
  }

  const char *query_type = "Table Scan";
  Record records[NUM_RECORDS];
  if (record_manager_scan_records(&ee->record_manager, records) !=
      NUM_RECORDS) {
    fprintf(stderr, "Error: Failed to table scan while executing query\n");
    return -1;
  }

  Record records_to_report[RECORD_REPORT_SIZE];
  size_t num_records_found = 0;
  for (size_t i = 0; i < NUM_RECORDS; i++) {
    if (records[i].random != v1) {
      records_to_report[num_records_found++] = records[i];
      if (num_records_found >= RECORD_REPORT_SIZE) {
        printf("Warning: More than %d (%zu) records found, only showing first "
               "%d\n",
               RECORD_REPORT_SIZE, num_records_found, RECORD_REPORT_SIZE);
        break;
      }
    }
  }
  for (size_t i = 0; i < num_records_found; i++) {
    record_print(&records_to_report[i]);
  }
  printf("Index type used: %s\n", query_type);
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
  case COMMAND_CREATE_INDEX:
    return execution_engine_create_index(ee);
  case COMMAND_EQUALITY_QUERY:
    return execution_engine_execute_equality_query(ee, plan.v1);
  case COMMAND_RANGE_QUERY:
    return execution_engine_execute_range_query(ee, plan.v1, plan.v2);
  case COMMAND_INEQUALITY_QUERY:
    return execution_engine_execute_inequality_query(ee, plan.v1);
  default:
    fprintf(stderr, "Error: Unknown command type.\n");
    return -1;
  }
}
