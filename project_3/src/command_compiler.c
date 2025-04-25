#include "command_compiler.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

#define HASH_JOIN_QUERY                                                        \
  "SELECT A.Col1, A.Col2, B.Col1, B.Col2 FROM A, B WHERE "                     \
  "A." DEFAULT_INDEX_COLUMN " = B." DEFAULT_INDEX_COLUMN

static Command query_to_command(const char *s) {
  if (s == NULL) {
    return COMMAND_UNKNOWN;
  }

  if (strncmp(s, HASH_JOIN_QUERY, strlen(HASH_JOIN_QUERY)) == 0) {
    return COMMAND_HASH_JOIN;
  } else if (strncmp(s, "SELECT Col2, ", strlen("SELECT Col2, ")) == 0) {
    return COMMAND_HASH_AGGREGATION;
  }
  return COMMAND_UNKNOWN;
}

/*
 * Reads one line of input from `stdin`.
 * Returns the number of characters read, or -1 if an error occurred.
 * `getline()` will allocate memory for `line` if it is NULL, which should be
 * freed by the user program even if `getline()` failed.
 */
static ssize_t input_getline(char **line) {
  if (line == NULL) {
    return -1;
  }

  printf("Program is ready and waiting for user command.\n");
  size_t size = 0;
  ssize_t chars_read;
  chars_read = getline(line, &size, stdin);
  return chars_read;
}

static int input_parse_and_validate_line(char *line, CommandArgs *args_out) {
  if (line == NULL || args_out == NULL) {
    return -1;
  }

  args_out->command_type = query_to_command(line);
  switch (args_out->command_type) {
  case COMMAND_HASH_JOIN:
    if (strncmp(line, HASH_JOIN_QUERY "\n", strlen(HASH_JOIN_QUERY "\n")) !=
        0) {
      return -1;
    }
    break;
  case COMMAND_HASH_AGGREGATION:
    // TODO: implement
    return -1;
    break;
  default:
    fprintf(stderr, "Error: Invalid command.\n");
    return -1;
  }

  return 0;
}

void command_compiler_init(CommandCompiler *cc) {
  if (cc == NULL) {
    return;
  }

  execution_engine_init(&cc->execution_engine);
}

void command_compiler_cleanup(CommandCompiler *cc) {
  if (cc == NULL) {
    return;
  }

  execution_engine_cleanup(&cc->execution_engine);
}

/**
 * Begin reading instructions and completing requested operations. Loops
 * continuously. Returns when finished.
 */
void command_compiler_start(CommandCompiler *cc) {
  if (cc == NULL) {
    return;
  }

  char *line = NULL;
  while (1) {
    if (input_getline(&line) < 1) {
      printf("End of file.\n");
      free(line);
      return;
    } else {

      struct timespec start_time;
      struct timespec end_time;
      struct timespec diff_time;
      get_time(&start_time);

      CommandArgs args = {
          .command_type = COMMAND_UNKNOWN,
      };

      if (input_parse_and_validate_line(line, &args) != -1) {
        QueryPlan plan = {
            .command_type = args.command_type,
        };
        if (execution_engine_execute_plan(&cc->execution_engine, plan) != -1) {
          get_time(&end_time);
          get_time_diff(&start_time, &end_time, &diff_time);
          long milliseconds =
              diff_time.tv_sec * 1000 + diff_time.tv_nsec / 1000000;
          printf("Time taken to answer the query: %ld ms\n", milliseconds);
        }
      }
    }
    free(line);
  }
}
