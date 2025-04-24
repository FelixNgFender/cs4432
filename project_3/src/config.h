#ifndef CONFIG_H
#define CONFIG_H

#define DATA_FILE_PREFIX "B"
#define DISK_DIR "./Project3Dataset/Project3Dataset-" DATA_FILE_PREFIX
#define NUM_BLOCKS 99
#define BLOCK_SIZE 4000
#define NUM_RECORDS_PER_BLOCK 100
#define NUM_RECORDS (NUM_RECORDS_PER_BLOCK * NUM_BLOCKS)

#define RECORD_SIZE 40
#define RECORD_OFFSET(record_id) ((record_id - 1) * RECORD_SIZE)

#define MIN_BLOCK_ID 1
#define MAX_BLOCK_ID 99
#define MIN_RECORD_ID 1
#define MAX_RECORD_ID 100
#define MIN_NAME 1
#define MAX_NAME 100
#define MIN_ADDRESS 1
#define MAX_ADDRESS 100
#define MIN_RANDOM 1
#define MAX_RANDOM 500

#define DEFAULT_INDEX_COLUMN "RandomV"
#define DEFAULT_TABLE_NAME "Project3Dataset"

#define HASH_INDEX_SIZE 250
#define ARRAY_INDEX_SIZE 500

#define RECORD_REPORT_SIZE NUM_RECORDS

#define BLOCK_ID(record_number) (record_number / NUM_RECORDS_PER_BLOCK + 1)
#define RECORD_ID(record_number) (record_number % NUM_RECORDS_PER_BLOCK)

#define DEFAULT_BUFFER_POOL_SIZE 10

#endif // CONFIG_H
