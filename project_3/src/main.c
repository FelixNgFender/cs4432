#include "database.h"
#include "util.h"

int main() {
  Database *db = check_malloc(sizeof(Database));
  database_init(db);
  database_start(db);
  database_cleanup(db);
  free(db);
  return EXIT_SUCCESS;
}
