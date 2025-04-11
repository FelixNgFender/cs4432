#ifndef MEM_H
#define MEM_H

#include <stdlib.h>

void *check_malloc(size_t size);
void *check_realloc(void *ptr, size_t size);

#endif // MEM_H
