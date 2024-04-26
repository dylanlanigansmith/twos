#pragma once
#include "../stdlib.h"
#include "alignment.h"
typedef struct {
    uint64_t entries[512];
} __attribute__((packed)) pt_t; //use this for ptrs existing



typedef struct {
    uint64_t entries[512];
} __attribute__((packed, aligned(4096))) page_table_t; //use this for creating them

#define VIRTMAP  0xfc000000//0xfc000000  //0xffff800000000000
#define PAGE_SIZE 0x200000 //2mib

//flags
//intel manual 3132-3133 redo these


#define PAGE_PRESENT_FLAG (1ULL) 
#define PAGE_WRITE_FLAG   (1ULL << 1) 
#define PAGE_USER         (1ULL << 2)
//check these
//intel manual page 3132

#define PAGE_SIZE_2MB     (1ULL << 7) // PSE flag



#define K_ID_MAP_START 0x00LLU
#define K_ID_MAP_END 0x40000000LLU
#define K_ID_MAP_SIZE (size_t)(K_ID_MAP_END - K_ID_MAP_START)

#define KERN_VAS_END KERNEL_ADDR + K_ID_MAP_SIZE

#define KERNEL_P4_IDX 511
#define KERNEL_P3_IDX 510

void paging_init();

size_t expand_heap(void* heap_ptr, size_t size_to_add);

uintptr_t virt_to_phys(uintptr_t virt);
//LOOK AT THIS!!!
//https://wiki.osdev.org/images/thumb/6/6b/64-bit_page_tables2.png/412px-64-bit_page_tables2.png

#define PAGE_FLAGS_DEFAULT 0b10000011LLU

#define PAGE_FLAGS_USER 0b100
uintptr_t map_phys_addr(uintptr_t virt, uintptr_t phys, size_t size, uint64_t flags);
uintptr_t unmap_phys_addr(uintptr_t virt,  size_t size);

typedef struct {
    page_table_t* p4;
    page_table_t* p3;
    page_table_t* p2;

    uintptr_t p4p, p3p,p2p;

} user_pt_t;

uintptr_t map_user_page_tables(uintptr_t virt, uintptr_t phys, size_t size, user_pt_t* pt, int fl);


static inline page_table_t* pt_addr(uintptr_t entry){ 
    return (page_table_t*)(entry & 0xFFFFFFFFFFFFFF00ull);
}


static inline uintptr_t round_up_to_page(uintptr_t addr){
    if(addr % PAGE_SIZE == 0) return addr;
    return (addr + (PAGE_SIZE - (addr % PAGE_SIZE)));
}

static inline int calc_num_pages(uint64_t addr_size){
    return (addr_size + PAGE_SIZE - 1) / PAGE_SIZE;
}
void invalidate_page(uintptr_t virtual_addr);
/* //fucking gcc cant handle this one
static inline uintptr_t swap_cr3(uintptr_t new_cr3){
    uintptr_t old_cr3 = 0;
    //lets just assume yr not dumb enough to call this with itrps on
    __asm__ volatile ("mov rax, cr3; mov %0, rax; mov rdi, %1;  mov cr3, rdi; " : "=r"(old_cr3): "r"(new_cr3):  "rdi"); //probs already is rdi lol
    return old_cr3;
}
*/
//todo:
//write more helpers for vaddr->phys translation stuff 




//OUT DATED BUT I LIKE READING MY OLD THOUGHT PROCESSES
// the big issue
    // we are gonna point page tables to phys addresses, and thats cool and all
    //  - except we gonna need to walk tables and uh 
        // we cant really do phys -> virtual 
        //Solution 1:
            //identity map wherever we are allocating page tables 

        //Solution 2:
            //recursive stuff

        //Solution 3:
            //complicated lut or algorithm to scrape tables 

        //Solution 4:
            //well if we know they are in kernel heap its pretty easy to find virt address 
            //just sucks a bit
        
    //go with #4, knowing we should do #2

    //yk it feels pretty wrong to stick page tables on the heap
    // just cuz like shit gonna break HARD if heap/malloc f's up and wipes a page table / chunk of one