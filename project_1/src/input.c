#include "input.h"
#include "instructions.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Args {
  unsigned char new_record[RECORD_SIZE];
  Instruction instruction_type;
  uint32_t record_number;
  uint8_t block_id;
} Args;

int input_parse_and_validate_line(char *line, Args *args_out);
int input_strtouint8(char *in_str, uint8_t *out_uint8);
int input_strtouint32(char *in_str, uint32_t *out_uint32);
int input_dispatch_instruction(Args *args, BufferPool *pool);
void handle_invalid_instruction(Instruction instruction_type);

int input_next_instruction(char *line, BufferPool *pool) {
  Args args = {
      .new_record = {0},
      .instruction_type = INSTRUCTION_UNKNOWN,
      .record_number = 0,
      .block_id = 0,
  };

  if (input_parse_and_validate_line(line, &args) == -1) {
    return -1;
  }

  return input_dispatch_instruction(&args, pool);
}

ssize_t input_getline(char **line) {
  printf("Instruction? ");
  size_t size = 0;
  ssize_t chars_read;
  chars_read = getline(line, &size, stdin);
  return chars_read;
}

int input_parse_and_validate_line(char *line, Args *args_out) {
  Instruction instruction_type;
  uint32_t record_number;
  char *record_number_str;
  uint8_t block_id;
  char *block_id_str;

  args_out->instruction_type = str_to_instruction(strtok(line, " "));
  switch (args_out->instruction_type) {
  case INSTRUCTION_GET: // GET <record_id>
    record_number_str = strtok(NULL, "\n");
    if (input_strtouint32(record_number_str, &record_number) == -1) {
      return -1;
    }
    if (record_number == 0) {
      fprintf(stderr,
              "Error: Invalid record number: %d. Record number must be greater "
              "than 0.\n",
              record_number);
      return -1;
    }

    args_out->record_number = record_number;
    break;
  case INSTRUCTION_SET: // SET <record_id> <new_record>
    record_number_str = strtok(NULL, " ");
    if (input_strtouint32(record_number_str, &record_number) == -1) {
      return -1;
    }
    if (record_number == 0) {
      fprintf(stderr,
              "Error: Invalid record number: %d. Record number must be greater "
              "than 0.\n",
              record_number);
      return -1;
    }

    char *new_record_str = strtok(NULL, "\n");
    if (new_record_str == NULL || strlen(new_record_str) >= RECORD_SIZE) {
      fprintf(
          stderr,
          "Error: Invalid or too large new record. Maximum size is %d bytes.\n",
          RECORD_SIZE - 1);
      return -1;
    }
    strncpy((char *)args_out->new_record, new_record_str, RECORD_SIZE);

    args_out->record_number = record_number;
    args_out->new_record[RECORD_SIZE - 1] = '\0'; // Ensure null termination
    break;
  case INSTRUCTION_PIN: // PIN <block_id>
    block_id_str = strtok(NULL, "\n");
    if (input_strtouint8(block_id_str, &block_id) == -1) {
      return -1;
    }
    if (block_id == 0) {
      fprintf(stderr,
              "Error: Invalid block ID: %d. Block ID must be greater than 0.\n",
              block_id);
      return -1;
    }

    args_out->block_id = block_id;
    break;
  case INSTRUCTION_UNPIN: // UNPIN <block_id>
    block_id_str = strtok(NULL, "\n");
    if (input_strtouint8(block_id_str, &block_id) == -1) {
      return -1;
    }
    if (block_id == 0) {
      fprintf(stderr,
              "Error: Invalid block ID: %d. Block ID must be greater than 0.\n",
              block_id);
      return -1;
    }

    args_out->block_id = block_id;
    break;
  default:
    handle_invalid_instruction(args_out->instruction_type);
    return -1;
  }

  return 0;
}

int input_strtouint8(char *in_str, uint8_t *out_uint8) {
  int bytes_cast;
  bytes_cast = sscanf(in_str, "%hhu", out_uint8);
  if (bytes_cast != 1) {
    fprintf(stderr, "Error: Incorrectly formatted instruction.\n");
    *out_uint8 = 0;
    return -1;
  }
  return 0;
}

int input_strtouint32(char *in_str, uint32_t *out_uint32) {
  int bytes_cast;
  bytes_cast = sscanf(in_str, "%u", out_uint32);
  if (bytes_cast != 1) {
    fprintf(stderr, "Error: Incorrectly formatted instruction.\n");
    *out_uint32 = 0;
    return -1;
  }
  return 0;
}

int input_dispatch_instruction(Args *args, BufferPool *pool) {
  Instruction instruction_type = args->instruction_type;
  switch (instruction_type) {
  case INSTRUCTION_GET:
    return instruction_get(pool, args->record_number);
  case INSTRUCTION_SET:
    return instruction_set(pool, args->record_number, args->new_record);
  case INSTRUCTION_PIN:
    return instruction_pin(pool, args->block_id);
  case INSTRUCTION_UNPIN:
    return instruction_unpin(pool, args->block_id);
  default:
    handle_invalid_instruction(instruction_type);
    return -1;
  }
}

void handle_invalid_instruction(Instruction instruction_type) {
  fprintf(stderr,
          "Error: Invalid instruction: %s. Valid instructions are GET, SET, "
          "PIN, and UNPIN.\n",
          instruction_to_str(instruction_type));
}
