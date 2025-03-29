#ifndef INPUT_H
#define INPUT_H

#include "buffer_pool.h"
#include <stdio.h>

/*
 * Reads one line of input from `stdin`.
 * Returns the number of characters read, or -1 if an error occurred.
 * `getline()` will allocate memory for `line` if it is NULL, which should be
 * freed by the user program even if `getline()` failed.
 */
ssize_t input_getline(char **line);
/*
 * Validates and responds to the next line of instruction.
 *
 * Returns 0 if the instruction was successfully executed, or -1 if an error
 */
int input_next_instruction(char *line, BufferPool *pool);

#endif // INPUT_H
