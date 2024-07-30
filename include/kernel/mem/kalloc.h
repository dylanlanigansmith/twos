#pragma once

#include <kstdlib.h>



#define HEAP_PHYS 0x80000000
#define HEAP_VIRT (0xffffffffc0000000llu - (uint64_t)(PAGE_SIZE * 128) ) //0xffffbeefed000000ULL
//dont fuckin ask
#define HEAP_VIRT_REAL HEAP_VIRT //0x007fbeef00000000 //0xffffbeef00000000
#define HEAP_SIZE 0x800000 //this is initial size


#define HEAP_MAX_SIZE (uint64_t)(PAGE_SIZE * 128) //(size_t)(HEAP_SIZE * 64) 

#define KMALLOC_FAIL 0




//could add index in bmap to header  ? 





void* kmalloc(size_t size); //look at linux kmalloc options https://stackoverflow.com/questions/20079767/what-is-different-functions-malloc-and-kmalloc


size_t kfree(void* address);

void kalloc_init();

//#define malloc(size) kmalloc(size)
//#define free(addr) kfree(addr)

