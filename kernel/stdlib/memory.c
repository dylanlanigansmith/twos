#include "memory.h"

void* memcpy(void* dest, void* src, size_t num_bytes){
    for(int i = 0; i < num_bytes; ++i)
        *((uint8_t*)dest + i) = *((uint8_t*)src + i);
    return dest;
}


void* memset(void* dest, uint8_t val, size_t num_bytes){
    for(int i = 0; i < num_bytes; ++i){
        *((char*)(dest + i)) = val;
    }
    return dest;
}