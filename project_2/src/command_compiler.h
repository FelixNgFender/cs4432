#ifndef COMMAND_COMPILER_H
#define COMMAND_COMPILER_H

#include "execution_engine.h"
#include <stdio.h>

typedef struct CommandCompiler {
  ExecutionEngine execution_engine;
} CommandCompiler;

void command_compiler_init(CommandCompiler *cc);
/**
 * Begin reading instructions and completing requested operations. Loops
 * continuously. Returns when finished.
 */
void command_compiler_start(CommandCompiler *cc);
void command_compiler_cleanup(CommandCompiler *cc);

#endif // COMMAND_COMPILER_H
