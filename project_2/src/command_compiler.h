#ifndef COMMAND_COMPILER_H
#define COMMAND_COMPILER_H

#include "execution_engine.h"

typedef struct CommandCompiler {
  ExecutionEngine execution_engine;
} CommandCompiler;

typedef struct CommandArgs {
  Command command_type;
  const char *table_name;
  const char *index_column;
  uint16_t v1;
  uint16_t v2;
} CommandArgs;

void command_compiler_init(CommandCompiler *cc);
void command_compiler_cleanup(CommandCompiler *cc);
void command_compiler_start(CommandCompiler *cc);

#endif // COMMAND_COMPILER_H
