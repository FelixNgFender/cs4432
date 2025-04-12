#include "execution_engine.h"
#include "index_manager.h"
#include <stdio.h>
#include <string.h>

int execution_engine_create_index(ExecutionEngine *ee);
int execution_engine_execute_equality_query(ExecutionEngine *ee, uint16_t v1);
int execution_engine_execute_range_query(const ExecutionEngine *ee, uint16_t v1,
                                         uint16_t v2);
int execution_engine_execute_inequality_query(const ExecutionEngine *ee,
                                              uint16_t v1);

void execution_engine_init(ExecutionEngine *ee) {
  if (ee == NULL) {
    return;
  }

  ee->index_manager_built = false;
  index_manager_init(&ee->index_manager);
}

void execution_engine_cleanup(ExecutionEngine *ee) {
  if (ee == NULL) {
    return;
  }

  index_manager_cleanup(&ee->index_manager);
}

QueryPlan query_plan_create(Command command_type, const char *table_name,
                            const char *index_column, uint16_t v1,
                            uint16_t v2) {
  if (command_type == COMMAND_UNKNOWN) {
    fprintf(stderr, "Error: Invalid command type.\n");
    return (QueryPlan){.command_type = COMMAND_UNKNOWN};
  }

  QueryPlan plan = {
      .command_type = command_type,
      .table_name = table_name,
      .index_column = index_column,
      .v1 = v1,
      .v2 = v2,
  };
  return plan;
}

int execution_engine_execute_plan(ExecutionEngine *ee, QueryPlan plan) {
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
    handle_invalid_command();
    return -1;
  }
}

int execution_engine_create_index(ExecutionEngine *ee) {
  if (ee == NULL) {
    fprintf(stderr, "Error: Invalid arguments to create index.\n");
    return -1;
  }

  const char *dirname = DISK_DIR;
  Record records[NUM_RECORDS] = {0};
  if (records_parse_from_dir(dirname, records) != NUM_RECORDS) {
    fprintf(stderr, "Error: Failed to parse %d records from directory %s\n",
            NUM_RECORDS, dirname);
    return -1;
  }
  index_manager_build(&ee->index_manager, records, NUM_RECORDS);
  ee->index_manager_built = true;

  return 0;
}

int execution_engine_execute_equality_query(ExecutionEngine *ee, uint16_t v1) {
  if (!ee->index_manager_built) {
    // TODO: full table scan
    printf("Used table scan.\n");
    return -1;
  }

  Record records[1] = {0};
  size_t num_records_queried = index_manager_get_records_with_key_range(
      &ee->index_manager, v1, 1, records);
  if (num_records_queried != 1) {
    fprintf(stderr, "Error: No entry found for value %u in index.\n", v1);
    return -1;
  }

  printf("Found %zu records with key %u:\n", num_records_queried, v1);
  record_print(&records[0]);
  return 0;
}

int execution_engine_execute_range_query(const ExecutionEngine *ee, uint16_t v1,
                                         uint16_t v2) {
  return -1;
}

int execution_engine_execute_inequality_query(const ExecutionEngine *ee,
                                              uint16_t v1) {
  return -1;
}

void handle_invalid_command() {
  fprintf(stderr, "Error: Invalid command. Valid instructions are "
                  "CREATE INDEX and SELECT.\n");
}

Command str_to_command(const char *s) {
  if (strncmp(s, "CREATE INDEX ON", strlen("CREATE INDEX ON")) == 0) {
    return COMMAND_CREATE_INDEX;
  } else if (strncmp(s, "SELECT * FROM", strlen("SELECT * FROM")) == 0) {
    if (strstr(s, "WHERE") != NULL) {
      if (strstr(s, "!=") != NULL) {
        return COMMAND_INEQUALITY_QUERY;
      } else if (strstr(s, "=") != NULL) {
        return COMMAND_EQUALITY_QUERY;
      } else if (strstr(s, ">") != NULL && strstr(s, "<") != NULL) {
        return COMMAND_RANGE_QUERY;
      }
    }
  }
  return COMMAND_UNKNOWN;
}
