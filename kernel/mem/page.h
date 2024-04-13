#pragma once
#include "../stdlib.h"

typedef struct {
    uint64_t entries[512];
} __attribute__((packed)) pt_t; //use this for ptrs existing



typedef struct {
    uint64_t entries[512];
} __attribute__((packed, aligned(4096))) page_table_t; //use this for creating them

#define VIRTMAP  0xfc000000//0xfc000000  //0xffff800000000000
#define PAGE_SIZE 0x200000 //2mib

//flags

#define PAGE_PRESENT_FLAG 0x1
#define PAGE_WRITE_FLAG   0x2
#define PSE_HUGE_FLAG     (1ULL << 7) // PSE flag


void make_page_struct();

size_t expand_heap(void* heap_ptr, size_t size_to_add);

uintptr_t virt_to_phys(uintptr_t virt);

uintptr_t map_phys_addr(uintptr_t virt, uintptr_t phys, size_t size);



static inline uintptr_t page_align(uintptr_t address) {
    return address & 0xFFFFFFFFFFF00000ull;
}

static inline bool is_page_aligned(uint64_t address) {
    return page_align(address) == address; //21 lsb == 0
}
