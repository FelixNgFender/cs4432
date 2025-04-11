#include "block.h"
#include "main_index.h"
#include "mem.h"
#include "util.h"

int main() {
  Record records[NUM_RECORDS] = {0};
  if ((records_parse_from_dir(records, DISK_DIR)) != NUM_RECORDS) {
    handle_error("Failed to parse records from directory");
  }
  MainIndex *index = check_malloc(sizeof(MainIndex));
  main_index_init(index);
  main_index_build(index, records, NUM_RECORDS);
  main_index_print(index);
  main_index_cleanup(index);
  free(index);

  return EXIT_SUCCESS;
}
