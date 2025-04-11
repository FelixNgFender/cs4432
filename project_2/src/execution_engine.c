#include "execution_engine.h"
#include "database.h"
#include "hash_index.h"
#include <stdio.h>
#include <string.h>

int execution_engine_create_index(ExecutionEngine *ee, const char *table_name,
                                  const char *index_column);
int execution_engine_execute_equality_query(const ExecutionEngine *ee,
                                            const char *table_name,
                                            const char *index_column,
                                            uint16_t v1);
int execution_engine_execute_range_query(const ExecutionEngine *ee,
                                         const char *table_name,
                                         const char *index_column, uint16_t v1,
                                         uint16_t v2);
int execution_engine_execute_inequality_query(const ExecutionEngine *ee,
                                              const char *table_name,
                                              const char *index_column,
                                              uint16_t v1);

void execution_engine_init(ExecutionEngine *ee) {
  if (ee == NULL) {
    return;
  }

  index_manager_init(&ee->index_manager);
  ee->index_manager_built = false;
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
    return execution_engine_create_index(ee, plan.table_name,
                                         plan.index_column);
  case COMMAND_EQUALITY_QUERY:
    return execution_engine_execute_equality_query(ee, plan.table_name,
                                                   plan.index_column, plan.v1);
  case COMMAND_RANGE_QUERY:
    return execution_engine_execute_range_query(
        ee, plan.table_name, plan.index_column, plan.v1, plan.v2);
  case COMMAND_INEQUALITY_QUERY:
    return execution_engine_execute_inequality_query(
        ee, plan.table_name, plan.index_column, plan.v1);
  default:
    handle_invalid_command();
    return -1;
  }
}

int execution_engine_create_index(ExecutionEngine *ee, const char *table_name,
                                  const char *index_column) {
  if (ee == NULL || table_name == NULL || index_column == NULL) {
    fprintf(stderr, "Error: Invalid arguments to create index.\n");
    return -1;
  }

  const char *dirname = DISK_DIR;
  Record records[NUM_RECORDS] = {0};
  if (records_parse_from_dir(records, dirname) != NUM_RECORDS) {
    fprintf(stderr, "Error: Failed to parse %d records from directory %s\n",
            NUM_RECORDS, dirname);
    return -1;
  }
  index_manager_build(&ee->index_manager, records, NUM_RECORDS);
  ee->index_manager_built = true;
  return 0;
}

int execution_engine_execute_equality_query(const ExecutionEngine *ee,
                                            const char *table_name,
                                            const char *index_column,
                                            uint16_t v1) {
  printf("Executing equality query on %s where %s = %u using %p\n", table_name,
         index_column, v1, (void *)ee);
  if (!ee->index_manager_built) {
    // TODO: full table scan
    printf("Used table scan.\n");
    return -1;
  }

  // use hash-based index
  const IndexEntry *entry = hash_index_get(&ee->index_manager.hash_index, v1);
  if (entry == NULL) {
    fprintf(stderr, "Error: No entry found for value %u in index.\n", v1);
    return -1;
  }
  // prepare large enough in-memory buffer
  // TODO: communicate with index manager also interface buffers blabla
  printf("Used hash-based index.\n");
  return -1;
}

int execution_engine_execute_range_query(const ExecutionEngine *ee,
                                         const char *table_name,
                                         const char *index_column, uint16_t v1,
                                         uint16_t v2) {
  printf("Executing range query on %s where %s between %u and %u using %p\n",
         table_name, index_column, v1, v2, (void *)ee);
  return -1;
}

int execution_engine_execute_inequality_query(const ExecutionEngine *ee,
                                              const char *table_name,
                                              const char *index_column,
                                              uint16_t v1) {
  printf("Executing inequality query on %s where %s != %u using %p\n",
         table_name, index_column, v1, (void *)ee);
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
