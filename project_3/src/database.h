#ifndef DATABASE_H
#define DATABASE_H

#include "command_compiler.h"

typedef struct Database {
  CommandCompiler command_compiler;
} Database;

void database_init(Database *db);
void database_cleanup(Database *db);
void database_start(Database *db);

#endif // DATABASE_H
