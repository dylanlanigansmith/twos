
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
Things hardcoded heap needs
    - physical memory manager
    holy shit we are doing it 


For Heap:

for each page 2mb 
- bitflags or whatever for memory within
    -find empty spot and use it
    -free finds what page, then marks spot in page empty
-no empty spot???
    -add another page! 

we need a data structure

the search can be somewaht bst if we split into pages (2mib)

what happens if malloc > 2mib?
    -> i guess alloc pages and pretend they arent different

    -> cant do that it gets stoopid fast

just do a map thing
with entries (alloced) vs (empty)

JUST NEED A MAP! 

*/

typedef struct {

} heap_page_t;

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
