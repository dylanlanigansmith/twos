#include "memory.h"

void memcpy(void* src, void* dest, size_t num_bytes){
    for(int i = 0; i < num_bytes; ++i)
        *((uint8_t*)dest + i) = *((uint8_t*)src + i);
}


void memset(void* dest, uint8_t val, size_t num_bytes){
    for(int i = 0; i < num_bytes; ++i){
        *((char*)(dest + i)) = val;
    }
    
}