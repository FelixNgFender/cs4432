#include <stdlib.h>

typedef struct Database {
  CommandCompiler command_compiler;
} Database;

void database_init(Database *db) {
  if (db == NULL) {
    return;
  }

  command_compiler_init(&db->command_compiler);
}

void database_cleanup(Database *db) {
  if (db == NULL) {
    return;
  }

  command_compiler_cleanup(&db->command_compiler);
}

void database_start(Database *db) {
  if (db == NULL) {
    return;
  }

  command_compiler_start(&db->command_compiler);
}
