#include "storage_manager.h"
#include "config.h"
#include "util.h"
#include <stdio.h>

char *table_to_str(Table table) {
  switch (table) {
  case TABLE_A:
    return "A";
  case TABLE_B:
    return "B";
  default:
    return "Unknown";
  }
}

static inline void get_block_path(Table table, uint8_t block_id,
                                  char *path_out) {
  if (table == TABLE_UNKNOWN || block_id == 0 || path_out == NULL) {
    handle_error("Invalid parameters for block path");
  }

  snprintf(path_out, FILENAME_MAX, "%s%s/%s%d.txt", DISK_DIR,
           table_to_str(table), table_to_str(table), block_id);
}

void storage_manager_read_block(Table table, uint8_t block_id,
                                unsigned char *dst) {
  if (table == TABLE_UNKNOWN || block_id == 0 || dst == NULL) {
    handle_error("Invalid parameters for reading block");
  }

  char block_path[FILENAME_MAX];
  get_block_path(table, block_id, block_path);

  FILE *block_fp = fopen(block_path, "r");
  fseek(block_fp, 0, SEEK_SET);
  if (fread(dst, 1, BLOCK_SIZE, block_fp) != BLOCK_SIZE) {
    handle_error("Error reading from block file");
  }
  fclose(block_fp);
}

void storage_manager_write_block(Table table, uint8_t block_id,
                                 const unsigned char *src) {
  if (table == TABLE_UNKNOWN || block_id == 0 || src == NULL) {
    handle_error("Invalid parameters for writing block");
  }

  char block_path[FILENAME_MAX];
  get_block_path(table, block_id, block_path);

  FILE *block_fp = fopen(block_path, "w");
  fseek(block_fp, 0, SEEK_SET);
  if (fwrite(src, 1, BLOCK_SIZE, block_fp) != BLOCK_SIZE) {
    handle_error("Error writing to block file");
  }
  fclose(block_fp);
}
