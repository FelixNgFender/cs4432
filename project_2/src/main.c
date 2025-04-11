#include "database.h"
#include <stdlib.h>

int main() {
  Database db;
  database_init(&db);
  database_start(&db);
  database_cleanup(&db);
  return EXIT_SUCCESS;
}
