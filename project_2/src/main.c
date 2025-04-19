#include "util.c"
#include "storage_manager.c"
#include "block.c"
#include "buffer_manager.c"
#include "record.c"
#include "array_index.c"
#include "hash_index.c"
#include "index_manager.c"
#include "record_manager.c"
#include "execution_engine.c"
#include "command_compiler.c"
#include "database.c"

int main() {
  Database *db = check_malloc(sizeof(Database));
  database_init(db);
  database_start(db);
  database_cleanup(db);
  free(db);
  return EXIT_SUCCESS;
}
