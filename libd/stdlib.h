#pragma once
#include "stdint.h"

#define HEAP_INITIAL_SIZE PAGE_SIZE*2

void* memcpy2(register void* dest, register void* src, register size_t num_bytes);
void * memcpy(void *dst0, const void *src0, size_t length);
void* memset(void* dest, register uint8_t val, register size_t num_bytes);

void* malloc(size_t size);

void free(void* p);


void* calloc( size_t num, size_t size );

void *realloc( void *ptr, size_t new_size );

static inline void *memmove(void *dest, const void *src, size_t n) { return memcpy(dest, src, n); } //so cursed 


void* mmap_file(const char* name);