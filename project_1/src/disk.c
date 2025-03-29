#include "disk.h"
#include "config.h"
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>

void get_block_path(uint8_t block_id, char *path_out) {
  snprintf(path_out, NAME_MAX, "%s/F%d.txt", DISK_DIR, block_id);
}

void disk_read_block(uint8_t block_id, unsigned char *dst) {
  char block_path[NAME_MAX];
  get_block_path(block_id, block_path);

  FILE *block_fp = fopen(block_path, "r");
  fseek(block_fp, 0, SEEK_SET);
  if (fread(dst, 1, BLOCK_SIZE, block_fp) != BLOCK_SIZE) {
    fprintf(stderr, "Error reading from block file\n");
    exit(EXIT_FAILURE);
  }
  fclose(block_fp);
}

void disk_write_block(uint8_t block_id, const unsigned char *src) {
  char block_path[NAME_MAX];
  get_block_path(block_id, block_path);

  FILE *block_fp = fopen(block_path, "w");
  fseek(block_fp, 0, SEEK_SET);
  if (fwrite(src, 1, BLOCK_SIZE, block_fp) != BLOCK_SIZE) {
    fprintf(stderr, "Error writing to block file\n");
    exit(EXIT_FAILURE);
  }
  fclose(block_fp);
}
