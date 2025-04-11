#include "record.h"
#include "database.h"
#include "util.h"
#include <dirent.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

size_t records_parse_from_file(Record *records, const char *filename);
void record_parse(Record *record, const unsigned char *data);

size_t records_parse_from_dir(Record *records, const char *dirname) {
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

    if ((records_parse_from_file(records + num_records, filename) !=
         NUM_RECORDS_PER_BLOCK)) {
      fprintf(stderr, "Failed to parse file: %s\n", filename);
      closedir(dir);
      return -1;
    }
    num_records += NUM_RECORDS_PER_BLOCK;
  }
  closedir(dir);
  return num_records;
}

size_t records_parse_from_file(Record *records, const char *filename) {
  if (records == NULL || filename == NULL) {
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

  size_t num_records = 0;
  // we can treat memory-mapped files like an array of bytes
  for (size_t record_idx = 0; record_idx < NUM_RECORDS_PER_BLOCK;
       ++record_idx) {
    off_t offset = RECORD_OFFSET(record_idx + 1);
    if (offset + RECORD_SIZE > st.st_size) {
      fprintf(stderr, "Unexpected end of file: %s\n", filename);
      break;
    }

    record_parse(&records[record_idx], (unsigned char *)file_data + offset);
    num_records++;
  }

  munmap(file_data, st.st_size);
  close(fd);
  return num_records;
}

void record_parse(Record *record, const unsigned char *data) {
  if (record == NULL || data == NULL) {
    return;
  }

  if (sscanf((const char *)data,
             "F%2hhu-Rec%3hhu, Name%3hhu, address%3hhu, %4hu...",
             &record->block_id, &record->record_id, &record->name,
             &record->address, &record->random) != 5) {
    fprintf(stderr, "Failed to parse record: %s\n", data);
    return;
  }

  if (record->block_id < MIN_BLOCK_ID || record->block_id > MAX_BLOCK_ID ||
      record->record_id < MIN_RECORD_ID || record->record_id > MAX_RECORD_ID ||
      record->name < MIN_NAME || record->name > MAX_NAME ||
      record->address < MIN_ADDRESS || record->address > MAX_ADDRESS ||
      record->random < MIN_RANDOM || record->random > MAX_RANDOM) {
    fprintf(stderr, "Invalid record values: %s\n", data);
    return;
  }

  return;
}
