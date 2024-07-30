#pragma once
#include <kernel/kernel.h>
#include <kernel/mem/page.h>

typedef struct {
    uint64_t entries[512];
} __attribute__((packed, aligned(4096))) page_table_t; 
typedef uintptr_t pagedir_t;

void flush_tlb();
void invalidate_page(uintptr_t virtual_addr);

void* virt_map_page(pagedir_t pt, phys_addr_t phys, uintptr_t virt, uint32_t flags);

void virt_unmap_page(pagedir_t pt, uintptr_t virt);


void virtmem_init();

pagedir_t* virt_current_pd();