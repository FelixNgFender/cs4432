#include "storage_manager.h"
#include "config.h"
#include "util.h"
#include <stdio.h>

static inline void get_block_path(uint8_t block_id, char *path_out) {
  snprintf(path_out, FILENAME_MAX, "%s/%s%d.txt", DISK_DIR, DATA_FILE_PREFIX,
           block_id);
}

void storage_manager_read_block(uint8_t block_id, unsigned char *dst) {
  char block_path[FILENAME_MAX];
  get_block_path(block_id, block_path);

  FILE *block_fp = fopen(block_path, "r");
  fseek(block_fp, 0, SEEK_SET);
  if (fread(dst, 1, BLOCK_SIZE, block_fp) != BLOCK_SIZE) {
    handle_error("Error reading from block file");
  }
  fclose(block_fp);
}

void storage_manager_write_block(uint8_t block_id, const unsigned char *src) {
  char block_path[FILENAME_MAX];
  get_block_path(block_id, block_path);

  FILE *block_fp = fopen(block_path, "w");
  fseek(block_fp, 0, SEEK_SET);
  if (fwrite(src, 1, BLOCK_SIZE, block_fp) != BLOCK_SIZE) {
    handle_error("Error writing to block file");
  }
  fclose(block_fp);
}
