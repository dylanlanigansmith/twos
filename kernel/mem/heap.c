
#include "heap.h"
#include "../../drivers/serial/serial.h"


#include "page.h"

#define DEBUG_HEAP
#ifdef DEBUG_HEAP
#define HEAPDBG(fmt, ...) oprintf( _COM, fmt, __VA_ARGS__)
#else
#define HEAPDBG(fmt, ...) ;
#endif
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



heap_t heap ={
    .addr = HEAP_VIRT_REAL,
    .size = HEAP_SIZE, //
    .allocs = nullptr
};







size_t get_total_block_size(size_t alloc_size){
    return (size_t)(sizeof(heap_footer_t) + sizeof(heap_header_t) + alloc_size);
}

uintptr_t get_heap_end(){//end address
    return (uintptr_t)(HEAP_VIRT_REAL) + heap.size;
} 

size_t get_heap_use(){ //used space
    return heap.addr - (uintptr_t)(HEAP_VIRT_REAL);
}

size_t get_heap_free(){ //free space
    return  get_heap_end() - heap.addr;
}

int get_heap_use_percent(){
    int64_t usage = (get_heap_use() * 10000LL ) / heap.size ;
    return (int)(usage / 100LL);
}

void create_heap_header(heap_header_t* header){
    header->crc = HEAP_CRC;
    header->available = HEAP_HEAD_IN_USE; //why make a new header if it aint in use
}
void create_heap_footer(heap_footer_t* footer, heap_header_t* header){
    footer->crc = HEAP_CRC;
    footer->header = header;
}

uintptr_t create_heap_block(uintptr_t addr, size_t mem_size){ //size is sizeof alloc, returns new end of heap
    heap_header_t* header = (heap_header_t*)(addr);
    create_heap_header(header);

    heap_footer_t* footer = (heap_footer_t*)(addr + mem_size + sizeof(heap_header_t));
    HEAPDBG("making footer @ %lx", (uintptr_t)footer);
    ASSERT((uintptr_t)footer < get_heap_end() );
    create_heap_footer(footer, header);

    header->block_size = (mem_size + sizeof(heap_footer_t));   

    return (uintptr_t)(footer) + sizeof(heap_footer_t); 
}


#define FREE_SPACE_NONE -1

#define ALLOC_FLAGS_NONE 0
uint32_t find_best_free_space(size_t size, uint8_t flags){
    if(heap.allocs->size == 0) return FREE_SPACE_NONE;
    uint32_t itr = 0;
    while(itr < heap.allocs->size){
        heap_header_t* header = (heap_header_t*)bmap_get(heap.allocs, itr);
        if(!header)  KPANIC("missing heap header");
        if(header->available == HEAP_HEAD_AVAILABLE){
            if( (size_t)(header->block_size - sizeof(heap_footer_t)) >= size ){
            
                HEAPDBG("found space at %i. need size %lx found size %lx \n", itr, size, header->block_size);

                /*
                * soo we have to make some decisions here, like if our space is > needed then we should split it up.. 
                * 
                */
                return itr;
            }

        }
        itr++;      
    }
    HEAPDBG("we found no space to fit size %lx \n", size);
    return FREE_SPACE_NONE;
}
//yeah i just wrote my own malloc what are you gonna do about it ?
void* _malloc(size_t size){
    uint32_t space = find_best_free_space(size, ALLOC_FLAGS_NONE);
    if(space == FREE_SPACE_NONE) // we need to add a new block
    {
        size_t total_size = get_total_block_size(size); //size of requested alloc + header & footer

        if( (total_size) > get_heap_free()){ //we must expand the heap!


            //HEAPDBG("heap is full, usage = %lx, size = %lx, free = %lx, \n need additional %lx for totalalloc %lx, alloc %lx  \n", get_heap_use(), heap.size, get_heap_free(), total_size - get_heap_free(), total_size, size);
            size_t expanded_size = expand_heap(&heap, total_size - get_heap_free());
            if(!expanded_size){
                KPANIC("Heap Full or Expansion Failed"); 
            }
            HEAPDBG("heap expanded! new size = %lx \n", heap.size);
            // HEAPDBG("heap addr %lx end addr %lx cur addr %lx \n", HEAP_VIRT_REAL, get_heap_end(), heap.addr);
        }
       
       // HEAPDBG("heap use now: %lx heap use new: %lx \n", (heap.addr - HEAP_VIRT_REAL), (heap.addr - HEAP_VIRT_REAL)  + total_size );

        uintptr_t new_header_addr = heap.addr; //header will be at top of heap
        uintptr_t alloc_addr = new_header_addr + sizeof(heap_header_t); //where our alloc will be after header is added
        HEAPDBG("making new block at %lx size=%lu / %lx \n", heap.addr, total_size, total_size);
        heap.addr = create_heap_block(heap.addr, size); //makes block at current end of heap, returns new end of heap after footer

        //last thing, we gotta add to map
       // HEAPDBG("adding to alloc map, prev. # allocs = %i \n", heap.allocs->size); 
        if(bmap_insert(heap.allocs, new_header_addr) != new_header_addr){
            HEAPDBG("%s", "bmap_insert failed to return expected address \n"); 
            KPANIC("heap header insertion failure");
        }
        HEAPDBG("added to alloc map, new # allocs = %i \n", heap.allocs->size); 

        HEAPDBG("Heap Usage: %i/100 \n", get_heap_use_percent());
        return alloc_addr; 
    } else{
        heap_header_t* header = (heap_header_t*)(bmap_get(heap.allocs, space));
        ASSERT(header != nullptr);
        ASSERT(header->crc == HEAP_CRC); //better than nothing
        HEAPDBG("using existing block at %lx / block_size=%lu \n", (uintptr_t)header, header->block_size);
        //we could check on our footer but surely that is fine!
        header->available = HEAP_HEAD_IN_USE;
        
        return (void*)((uintptr_t)header + sizeof(header) );
    }
}

size_t _free(void* addr){
    //so ideally as we free we merge blocks and resort as we come across them
    //for now just marking block empty will suffice
    if(addr == nullptr) return 0;
    heap_header_t* header =  (heap_header_t*)( (uintptr_t)addr - sizeof(heap_header_t) );
    heap_footer_t* footer = (heap_header_t*)( (uintptr_t)addr + header->block_size - sizeof(heap_footer_t));
    HEAPDBG("freeing block at %lx / block_size=%lu \n", (uintptr_t)header, header->block_size);
    //lets make sure we ok
    ASSERT(header->crc == footer->crc);
    ASSERT(footer->header == header);

    //okay we didnt fuck that up

    header->available = HEAP_HEAD_AVAILABLE; 

    //this is where we can just go back to footer above us and merge, etc
    
    HEAPDBG("block freed, size of allocs = %i \n", heap.allocs->size);

    //debug
    

    HEAPDBG("heap use current: %lx heap remaining: %lx \n", get_heap_use() , get_heap_free() );

}








uintptr_t resize_heap(){
    serial_println("resizing heap. HA YOU WISH");
    return 0;
}
uintptr_t kmalloc_bootstrap(uint64_t size)
{
    uintptr_t ret  = heap.addr;
    heap.addr += size;
    if(heap.addr > (heap.addr + heap.size)){
        if(!resize_heap())
            return KMALLOC_FAIL;
    }

    return ret;
}

void* kmalloc(size_t size)
{
    return _malloc(size);
}

size_t kfree(void* address)
{
    return _free(address);
}

uint8_t alloc_greater(void* a, void* b){

    heap_header_t* aa = (heap_header_t*)(a);
    heap_header_t* bb = (heap_header_t*)(b);
    return aa->block_size > bb->block_size;
}

void alloc_init()
{
    bmap_greater_fn greater_fn = &alloc_greater;
    heap.allocs = bmap_init(PAGE_SIZE, &kmalloc_bootstrap, &kfree, greater_fn);
    bmap_print_info(heap.allocs);
}
