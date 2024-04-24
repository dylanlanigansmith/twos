#include "stdlib.h"

void* memcpy(void* dest, void* src, size_t num_bytes){
    char *d = dest;
    const char *s = src;
    while (num_bytes--)
        *d++ = *s++;
    return dest;
}


void* memset(void* dest, register uint8_t val, register size_t num_bytes){
    register uint8_t *d = (uint8_t*)dest;
    while(num_bytes-- > 0)
        *d++ = val;
    return dest;
}