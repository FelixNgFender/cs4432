#ifndef DATABASE_H
#define DATABASE_H

#include "command_compiler.h"
#include "execution_engine.h"
#include "index_manager.h"

#define DISK_DIR "./Project2Dataset"
#define DATA_FILE_PREFIX 'F'
#define NUM_BLOCKS 99
#define BLOCK_SIZE 4000
#define NUM_RECORDS_PER_BLOCK 100
#define NUM_RECORDS (NUM_RECORDS_PER_BLOCK * NUM_BLOCKS)

typedef struct Database {
  CommandCompiler command_compiler;
} Database;

void database_init(Database *db);
void database_start(Database *db);
void database_cleanup(Database *db);

#endif // DATABASE_H
