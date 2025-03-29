#ifndef CONFIG_H
#define CONFIG_H

#define DISK_DIR "./Project1"
#define BLOCK_SIZE 4000
#define NUM_RECORDS_PER_BLOCK 100
#define RECORD_SIZE 40
#define RECORD_OFFSET(record_id) ((record_id - 1) * RECORD_SIZE)

#endif
