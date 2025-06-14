#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stddef.h>

void mem_init(size_t size);
void* mem_alloc(size_t size);
void mem_free(void* ptr);
void* mem_resize(void* ptr, size_t size);
void mem_deinit(void);

#endif
