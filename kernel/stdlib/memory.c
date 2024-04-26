#include "memory.h"

void* memcpy(void* dest, void* src, size_t num_bytes){
     if(!dest || !num_bytes || !src) return 0;
    char *d = dest;
    const char *s = src;
    while (num_bytes--)
        *d++ = *s++;
    return dest;
}


void* memset(void* dest, register uint8_t val, register size_t num_bytes){
    if(!dest ) return 0;
    register uint8_t *d = (uint8_t*)dest;
    while(num_bytes-- > 0)
        *d++ = val;
    return dest;
}

void* memset_old(void* dest, uint8_t val, size_t num_bytes){ 
    //so i remember it being like super late and i wrote this like when i first had C going just to test something quick

    //anyhow it ended up staying for a LONG time 
    //and it is so shitty and since stuff goes on stack
    //can get totally screwed by interupts 
    //google memset async signal safe 
    //anyways the register keyword make da problems go away
    //keeping this here to remind myself not to write shitty code 
    //doubt it will work 
    for(int i = 0; i < num_bytes; ++i){ //dunno
        *((char*)(dest + i)) = val;
    }
    return dest;
}


//REWRITE THIS HOLY 
void *memset_u32(volatile uint32_t* dest, const uint32_t val, const size_t num_ints) //so we pass size as number of 4 byte chunks to write
{
    __asm__ volatile ("cli"); //todo: investigate if we need this w/ registers 
    //todo alignment
    if(!dest || !num_ints) return nullptr;
    
    for(int i = 0; i < num_ints; ++i){
        dest[i] = val;
    }
      __asm__ volatile ("sti");
    return (void*)dest;
}
