#include "command_compiler.h"
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INDEX_COLUMN "RandomV"
#define DEFAULT_TABLE_NAME "Project2Dataset"

typedef struct CommandArgs {
  Command command_type;
  const char *table_name;
  const char *index_column;
  uint16_t v1;
  uint16_t v2;
} CommandArgs;

int input_parse_and_validate_line(char *line, CommandArgs *args_out);
/*
 * Reads one line of input from `stdin`.
 * Returns the number of characters read, or -1 if an error occurred.
 * `getline()` will allocate memory for `line` if it is NULL, which should be
 * freed by the user program even if `getline()` failed.
 */
ssize_t input_getline(char **line);

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

      CommandArgs args = {
          .command_type = COMMAND_UNKNOWN,
          .table_name = DEFAULT_TABLE_NAME,
          .index_column = DEFAULT_INDEX_COLUMN,
          .v1 = 0,
          .v2 = 0,
      };

      if (input_parse_and_validate_line(line, &args) == -1) {
        printf("Error: Invalid command.\n");
        free(line);
        continue;
      }

      QueryPlan plan = query_plan_create(args.command_type, args.table_name,
                                         args.index_column, args.v1, args.v2);
      if (execution_engine_execute_plan(&cc->execution_engine, plan) == -1) {
        fprintf(stderr, "Error: Failed to execute plan.\n");
      }
    }
    free(line);
  }
}

ssize_t input_getline(char **line) {
  printf("Program is ready and waiting for user command.\n");
  size_t size = 0;
  ssize_t chars_read;
  chars_read = getline(line, &size, stdin);
  return chars_read;
}

int input_parse_and_validate_line(char *line, CommandArgs *args_out) {
  uint16_t v1 = 0;
  uint16_t v2 = 0;

  args_out->command_type = str_to_command(line);
  switch (args_out->command_type) {
  case COMMAND_CREATE_INDEX:
    // CREATE INDEX ON Project2Dataset (RandomV)
    if (strncmp(line, "CREATE INDEX ON Project2Dataset (RandomV)\n",
                strlen("CREATE INDEX ON Project2Dataset (RandomV)\n")) != 0) {
      fprintf(stderr,
              "Error: Incorrectly formatted command. Expected format: "
              "CREATE INDEX ON %s (%s)\n",
              args_out->table_name, args_out->index_column);
      return -1;
    }
    break;
  case COMMAND_EQUALITY_QUERY:
    // SELECT * FROM Project2Dataset WHERE RandomV = 5
    if (sscanf(line, "SELECT * FROM Project2Dataset WHERE RandomV = %4hu\n",
               &v1) != 1) {
      fprintf(stderr,
              "Error: Incorrectly formatted command. Expected format: "
              "SELECT * FROM %s WHERE %s = <value>\n",
              DEFAULT_TABLE_NAME, DEFAULT_INDEX_COLUMN);
      return -1;
    }
    args_out->v1 = v1;
    break;
  case COMMAND_INEQUALITY_QUERY:
    // SELECT * FROM Project2Dataset WHERE RandomV != 5
    if (sscanf(line, "SELECT * FROM Project2Dataset WHERE RandomV != %4hu\n",
               &v1) != 1) {
      fprintf(stderr,
              "Error: Incorrectly formatted command. Expected format: "
              "SELECT * FROM %s WHERE %s != <value>\n",
              DEFAULT_TABLE_NAME, DEFAULT_INDEX_COLUMN);
      return -1;
    }
    args_out->v1 = v1;
    break;
  case COMMAND_RANGE_QUERY:
    // SELECT * FROM Project2Dataset WHERE RandomV > 5 AND RandomV < 10
    if (sscanf(line,
               "SELECT * FROM Project2Dataset WHERE RandomV > %4hu AND RandomV "
               "< %4hu\n",
               &v1, &v2) != 2) {
      fprintf(stderr,
              "Error: Incorrectly formatted command. Expected format: "
              "SELECT * FROM %s WHERE %s > <value> AND %s < <value>\n",
              DEFAULT_TABLE_NAME, DEFAULT_INDEX_COLUMN, DEFAULT_INDEX_COLUMN);
      return -1;
    }
    if (v1 >= v2) {
      fprintf(stderr,
              "Error: Invalid range. Expected format: "
              "SELECT * FROM %s WHERE %s > <value> AND %s < <value>\n",
              DEFAULT_TABLE_NAME, DEFAULT_INDEX_COLUMN, DEFAULT_INDEX_COLUMN);
      return -1;
    }
    args_out->v1 = v1;
    args_out->v2 = v2;
    break;
  default:
    handle_invalid_command();
    return -1;
  }

  return 0;
}
