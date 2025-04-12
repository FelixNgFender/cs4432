#include "storage_manager.h"
#include "util.h"
#include <dirent.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

size_t storage_manager_parse_records_from_file(const char *filename,
                                               Record *records_out);
size_t storage_manager_parse_record_from_buffer(const unsigned char *src,
                                                Record *record_out);

static inline void get_block_path(uint8_t block_id, char *path_out) {
  snprintf(path_out, FILENAME_MAX, "%s/F%d.txt", DISK_DIR, block_id);
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

void record_print(const Record *record) {
  if (record == NULL) {
    return;
  }

  printf("F%02hhu-Rec%03hhu, Name%03hhu, address%03hhu, %04hu...\n",
         record->block_id, record->record_id, record->name, record->address,
         record->random);
}

size_t records_parse_from_dir(const char *dirname, Record *records_out) {
  if (dirname == NULL) {
    return -1;
  }

  DIR *dir = opendir(dirname);
  if (dir == NULL) {
    handle_error("Error opening directory");
  }

  struct dirent *entry = NULL;
  off_t num_records = 0;
  while ((entry = readdir(dir)) != NULL) {
    char filename[FILENAME_MAX] = {0};
    snprintf(filename, FILENAME_MAX, "%s/%s", dirname, entry->d_name);

    // skip if directory
    if (entry->d_type == DT_DIR || entry->d_name[0] != DATA_FILE_PREFIX) {
      continue;
    }

    if ((storage_manager_parse_records_from_file(
             filename, records_out + num_records) != NUM_RECORDS_PER_BLOCK)) {
      fprintf(stderr, "Failed to parse file: %s\n", filename);
      closedir(dir);
      return -1;
    }
    num_records += NUM_RECORDS_PER_BLOCK;
  }
  closedir(dir);
  return num_records;
}

size_t storage_manager_parse_records_from_file(const char *filename,
                                               Record *records_out) {
  if (records_out == NULL || filename == NULL) {
    return -1;
  }

  int fd = open(filename, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "Error opening file: %s\n", filename);
    return -1;
  }

  struct stat st;
  if (fstat(fd, &st) < 0) {
    fprintf(stderr, "Error getting file size: %s\n", filename);
    close(fd);
    return -1;
  }

  void *file_data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (file_data == MAP_FAILED) {
    fprintf(stderr, "Error mapping file: %s\n", filename);
    close(fd);
    return -1;
  }

  // lets the kernel aggressively prefetch upcoming pages, speeding up I/O
  // and reducing page faults
  if (posix_madvise(file_data, st.st_size, POSIX_MADV_SEQUENTIAL) != 0) {
    fprintf(stderr, "Error advising memory mapping: %s\n", filename);
    munmap(file_data, st.st_size);
    close(fd);
    return -1;
  }

  // we can treat memory-mapped files like an array of bytes
  size_t num_records_parsed = 0;
  if ((num_records_parsed = storage_manager_parse_records_from_buffer(
           (unsigned char *)file_data, NUM_RECORDS_PER_BLOCK, records_out)) !=
      NUM_RECORDS_PER_BLOCK) {
    fprintf(stderr, "Failed to parse records from file: %s\n", filename);
    munmap(file_data, st.st_size);
    close(fd);
    return -1;
  }

  munmap(file_data, st.st_size);
  close(fd);
  return num_records_parsed;
}

size_t storage_manager_parse_records_from_buffer(const unsigned char *src,
                                                 size_t num_records,
                                                 Record *record_out) {
  if (record_out == NULL || src == NULL) {
    return -1;
  }

  size_t num_records_parsed = 0;
  for (size_t i = 0; i < num_records; ++i) {
    if (storage_manager_parse_record_from_buffer(src + RECORD_OFFSET(i + 1),
                                                 &record_out[i]) != 1) {
      fprintf(stderr, "Failed to parse record from buffer\n");
      break;
    }
    num_records_parsed++;
  }

  return num_records_parsed;
}

size_t storage_manager_parse_record_from_buffer(const unsigned char *src,
                                                Record *record_out) {
  if (record_out == NULL || src == NULL) {
    return -1;
  }

  if (sscanf((const char *)src,
             "F%2hhu-Rec%3hhu, Name%3hhu, address%3hhu, %4hu...",
             &record_out->block_id, &record_out->record_id, &record_out->name,
             &record_out->address, &record_out->random) != 5) {
    fprintf(stderr, "Failed to parse record: %s\n", src);
    return -1;
  }

  if (record_out->block_id < MIN_BLOCK_ID ||
      record_out->block_id > MAX_BLOCK_ID ||
      record_out->record_id < MIN_RECORD_ID ||
      record_out->record_id > MAX_RECORD_ID || record_out->name < MIN_NAME ||
      record_out->name > MAX_NAME || record_out->address < MIN_ADDRESS ||
      record_out->address > MAX_ADDRESS || record_out->random < MIN_RANDOM ||
      record_out->random > MAX_RANDOM) {
    fprintf(stderr, "Invalid record values: %s\n", src);
    return -1;
  }

  return 1;
}
