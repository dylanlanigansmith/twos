#pragma once
#include "stdint.h"

void* memcpy(void* dest, void* src, size_t num_bytes);

void* memset(void* dest, register uint8_t val, register size_t num_bytes);

void* malloc(size_t size);

void free(void* p);


void* calloc( size_t num, size_t size );

void *realloc( void *ptr, size_t new_size );

static inline void *memmove(void *dest, const void *src, size_t n) { return memcpy(dest, src, n); } //so cursed 