#include "memory.h"

void* memcpy(void* dest, void* src, size_t num_bytes){
    for(int i = 0; i < num_bytes; ++i)
        *((uint8_t*)dest + i) = *((uint8_t*)src + i);
    return dest;
}


void* memset(void* dest, register uint8_t val, register size_t num_bytes){
    register uint8_t *d = (uint8_t*)dest;
    while(num_bytes-- > 0)
        *d++ = val;
    return dest;
}

void* memset_old(void* dest, uint8_t val, size_t num_bytes){
    for(int i = 0; i < num_bytes; ++i){ //dunno
        *((char*)(dest + i)) = val;
    }
    return dest;
}

void *memset_u32(volatile uint32_t* dest, const uint32_t val, const size_t num_ints) //so we pass size as number of 4 byte chunks to write
{
    __asm__ volatile ("cli");
    //todo alignment
    if(!dest || !num_ints) return nullptr;
    
    for(int i = 0; i < num_ints; ++i){
        dest[i] = val;
    }
      __asm__ volatile ("sti");
    return (void*)dest;
}
