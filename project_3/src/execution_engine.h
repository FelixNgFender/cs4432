#ifndef EXECUTION_ENGINE_H
#define EXECUTION_ENGINE_H

#include "index_manager.h"
#include "record_manager.h"
#include <stdint.h>

typedef enum Command {
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
  RecordManager record_manager;
} ExecutionEngine;

void execution_engine_init(ExecutionEngine *ee);
void execution_engine_cleanup(ExecutionEngine *ee);
int execution_engine_execute_plan(ExecutionEngine *ee, QueryPlan plan);

#endif // EXECUTION_ENGINE_H
