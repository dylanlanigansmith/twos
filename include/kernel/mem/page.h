#pragma once
#include <stdint.h>
#define PAGE_SIZE 0x200000

//from randos
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


static inline uintptr_t round_up_to_page(uintptr_t addr){
    if(addr % PAGE_SIZE == 0) return addr;
    return (addr + (PAGE_SIZE - (addr % PAGE_SIZE)));
}

static inline int calc_num_pages(uint64_t addr_size){
    return (addr_size + PAGE_SIZE - 1) / PAGE_SIZE;
}