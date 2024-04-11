#pragma once
#ifdef __cplusplus
extern "C" {
#include "../stdlib/stdint.h" 
#else
#include "../stdlib.h"
#include "../stdlib/map.h"
#endif


#define HEAP_PHYS 0x80000000
#define HEAP_VIRT 0xdeadbeef00000000
//dont fuckin ask
#define HEAP_VIRT_REAL 0xffffbeef00000000
#define HEAP_SIZE 0x800000 //this is initial size

//temporary
#define HEAP_MAX_SIZE (size_t)(HEAP_SIZE * 64) 

#define KMALLOC_FAIL 0

#ifndef __cplusplus
typedef struct {
    uintptr_t addr;
    size_t size;
    bmap_t* allocs;
} heap_t;

//HEADER AND FOOTER
#define HEAP_CRC 0xD515DEAD

#define HEAP_HEAD_AVAILABLE (uint8_t)0xff
#define HEAP_HEAD_IN_USE (uint8_t)0x00
typedef struct {
    uint32_t crc;
    uint8_t available;
    size_t block_size; //size from end of header to end of footer 
} __attribute__((packed)) heap_header_t;

typedef struct {
    uint32_t crc;
    heap_header_t* header;
} __attribute__((packed)) heap_footer_t;
//could add index in bmap to header  ? 
#endif



void* _malloc(size_t size);

size_t _free(void* addr);

void* kmalloc(size_t size);

size_t kfree(void* address);

void alloc_init();

#ifdef __cplusplus
}
#endif
