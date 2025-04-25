#ifndef EXECUTION_ENGINE_H
#define EXECUTION_ENGINE_H

#include "aggregate_table.h"
#include "index_manager.h"
#include "record_manager.h"
#include <stdint.h>

typedef enum Command {
  COMMAND_UNKNOWN,
  COMMAND_HASH_JOIN,
  COMMAND_HASH_AGGREGATE,
} Command;

typedef struct QueryPlan {
  const Command command_type;
  const Table aggregate_table;
  const AggregateFn aggregate_fn;
} QueryPlan;

typedef struct ExecutionEngine {
  IndexManager index_manager;
  RecordManager record_manager;
} ExecutionEngine;

void execution_engine_init(ExecutionEngine *ee);
void execution_engine_cleanup(ExecutionEngine *ee);
int execution_engine_execute_plan(ExecutionEngine *ee, QueryPlan plan);

#endif // EXECUTION_ENGINE_H
