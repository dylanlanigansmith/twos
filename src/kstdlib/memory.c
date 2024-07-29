#include <kstdlib/memory.h>


void* memcpy(void* dest, const void* src, size_t n){
    uint8_t* d = dest;
    const uint8_t* s = src;
    while(n--)
        *d++ = *s++;
    return dest; 
}


void* memset(void* dest, uint8_t val, size_t n){
    uint8_t* d = dest;
    while(n--)
        *d++ = val;
    return dest;
}

