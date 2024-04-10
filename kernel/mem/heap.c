
#include "heap.h"
#include "../../drivers/serial/serial.h"


typedef struct {
    uintptr_t addr;
    uintptr_t size;
} heap_t;

heap_t heap ={
    .addr = HEAP_VIRT_REAL,
    .size = HEAP_SIZE //
};
/*
For Heap:

for each page 2mb 
- bitflags or whatever for memory within
    -find empty spot and use it
    -free finds what page, then marks spot in page empty
-no empty spot???
    -add another page! 



*/

uintptr_t resize_heap(){
    serial_println("resizing heap. HA YOU WISH");
    return 0;
}

uintptr_t kmalloc(uint64_t size)
{
    uintptr_t ret  = heap.addr;
    heap.addr += size;
    if(heap.addr > (heap.addr + HEAP_SIZE)){
        if(!resize_heap())
            return KMALLOC_FAIL;
    }

    return ret;
}

uintptr_t kfree(uint64_t address)
{
    serial_println("freeing. HA YOU WISH");
    return 0; //bytes freed
}
