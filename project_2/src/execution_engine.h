#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "index_manager.h"
#include <stdbool.h>
#include <stdint.h>

typedef enum {
  COMMAND_UNKNOWN,
  COMMAND_CREATE_INDEX,
  COMMAND_EQUALITY_QUERY,
  COMMAND_RANGE_QUERY,
  COMMAND_INEQUALITY_QUERY
} Command;

typedef struct QueryPlan {
  const Command command_type;
  const char *table_name;
  const char *index_column;
  const uint16_t v1;
  const uint16_t v2;
} QueryPlan;

typedef struct ExecutionEngine {
  IndexManager index_manager;
  bool index_manager_built;
} ExecutionEngine;

void execution_engine_init(ExecutionEngine *ee);
void execution_engine_cleanup(ExecutionEngine *ee);
int execution_engine_execute_plan(ExecutionEngine *ee, QueryPlan plan);

QueryPlan query_plan_create(Command command_type, const char *table_name,
                            const char *index_column, uint16_t v1, uint16_t v2);

void handle_invalid_command();
Command str_to_command(const char *s);
#endif // INSTRUCTIONS_H
