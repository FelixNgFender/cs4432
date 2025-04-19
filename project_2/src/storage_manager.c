#include "config.h"
#include <stdint.h>
#include <stdio.h>

static inline void get_block_path(uint8_t block_id, char *path_out) {
  snprintf(path_out, FILENAME_MAX, "%s/F%d.txt", DISK_DIR, block_id);
}

/**
 * Read `BLOCK_SIZE` bytes from disk block `block_id` into `dst`.
 */
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

/**
 * Write `BLOCK_SIZE` bytes from `src` into disk block `block_id`.
 */
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
