#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "buffer_pool.h"
#include <stdint.h>

typedef enum {
  INSTRUCTION_UNKNOWN,
  INSTRUCTION_GET,
  INSTRUCTION_SET,
  INSTRUCTION_PIN,
  INSTRUCTION_UNPIN,
} Instruction;

Instruction str_to_instruction(const char *s);
const char *instruction_to_str(Instruction i);

int instruction_get(BufferPool *pool, uint32_t record_number);
int instruction_set(BufferPool *pool, uint32_t record_number,
                    const unsigned char *new_record);
int instruction_pin(BufferPool *pool, uint8_t block_id);
int instruction_unpin(BufferPool *pool, uint8_t block_id);

#endif // INSTRUCTIONS_H
