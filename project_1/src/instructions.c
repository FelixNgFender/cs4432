#include "instructions.h"
#include "buffer_manager.h"
#include "frame.h"
#include <string.h>

int instruction_get(BufferPool *pool, uint32_t record_number) {
  uint8_t block_id = BLOCK_ID(record_number);
  uint8_t record_id = RECORD_ID(record_number);
  const Frame *frame = buffer_pool_get_frame(pool, block_id);
  const unsigned char *record = frame_get_record(frame, record_id);
  print_record(record);
  return 0;
}

int instruction_set(uint32_t record_number, const unsigned char *new_record) {}

int instruction_pin(uint8_t block_id) {}

int instruction_unpin(uint8_t block_id) {}

const char *instruction_to_str(Instruction i) {
  switch (i) {
  case INSTRUCTION_GET:
    return "GET";
  case INSTRUCTION_SET:
    return "SET";
  case INSTRUCTION_PIN:
    return "PIN";
  case INSTRUCTION_UNPIN:
    return "UNPIN";
  default:
    return "UNKNOWN";
  }
}

Instruction str_to_instruction(const char *s) {
  if (strncmp(s, "GET", strlen("GET")) == 0) {
    return INSTRUCTION_GET;
  } else if (strncmp(s, "SET", strlen("SET")) == 0) {
    return INSTRUCTION_SET;
  } else if (strncmp(s, "PIN", strlen("PIN")) == 0) {
    return INSTRUCTION_PIN;
  } else if (strncmp(s, "UNPIN", strlen("UNPIN")) == 0) {
    return INSTRUCTION_UNPIN;
  } else {
    return INSTRUCTION_UNKNOWN;
  }
}
