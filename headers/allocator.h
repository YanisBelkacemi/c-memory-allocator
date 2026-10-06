#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>

void my_free(void *ptr);
void* my_malloc(size_t size);
void* my_calloc(size_t size);
void* my_realloc(void* ptr , size_t size);
void malloc_init();
void malloc_destroy(void);
#endif