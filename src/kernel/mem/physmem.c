#include <kernel/mem/physmem.h>
#include <boot/multiboot2.h>
#include <kernel/printk.h>

//up to 128gb total memory support with 1024 not bad
//dont need it for dev
#define PMM_SIZE 512
uint64_t pmm[PMM_SIZE];
struct {
    struct{
        struct multiboot_tag_mmap* mmaps;
        size_t size;
    }mb;
  
    size_t mm_end;
    phys_addr_t end;
    size_t total_avail;
} physmem;



uint8_t get_phys(phys_addr_t addr){
    const size_t div = physmem.end / physmem.mm_end;
     size_t page_num = addr / PAGE_SIZE;
    size_t idx = page_num / (sizeof(uint64_t) * CHAR_BIT);
    size_t bit = page_num % (sizeof(uint64_t) * CHAR_BIT);
    
    return (pmm[idx] >> bit) & 1;
}
void set_phys(phys_addr_t addr, uint8_t b){
    size_t page_num = addr / PAGE_SIZE;
    size_t idx = page_num / (sizeof(uint64_t) * CHAR_BIT);
    size_t bit = page_num % (sizeof(uint64_t) * CHAR_BIT);
    uint64_t mask = (1 << bit);
    if(b)
        pmm[idx] |= mask; 
    else
        pmm[idx] &= ~mask; 

   // if( addr < 0xbffe0000 + PAGE_SIZE)
    //debugf("for addr %lx we set %i in [%li] b%li  %lb", addr, b, idx, bit, pmm[idx]);

}
void physmem_markregion(phys_addr_t map_start, phys_addr_t map_end, uint8_t b ){
       size_t num_pages = round_up_to_page(map_end) / PAGE_SIZE;
        for(phys_addr_t addr = map_start; addr < map_end; addr += PAGE_SIZE)        //obvs performace improvements incoming
            set_phys(addr, b);
        
}

size_t physmem_init(void *mb_mmap, size_t mmap_size)
{
    memset(&physmem,0, sizeof(physmem));
    memset(&pmm[0], PHYS_USED, sizeof(pmm) );
    struct multiboot_tag_mmap *mmaps = (struct multiboot_tag_mmap *)mb_mmap;
    physmem.mb.mmaps = mmaps;
    physmem.mb.size = mmap_size;

    
    phys_addr_t highest_avail = physmem.total_avail = 0;
    for (int i = 0; i < mmap_size; ++i){
        multiboot_memory_map_t mmap = mmaps->entries[i];
        if(mmap.type == MULTIBOOT_MEMORY_AVAILABLE) physmem.total_avail += mmap.len;

        if( (mmap.addr + mmap.len) > highest_avail && mmap.type == MULTIBOOT_MEMORY_AVAILABLE)
            highest_avail = (mmap.addr + mmap.len);
        debugf("mmap [%i]:{ @%lx - %lx } size %lx |  %i [%s] ", i, mmap.addr, mmap.addr + mmap.len, mmap.len, mmap.type, (mmap.type == 1) ? ("AVAIL") : "USED");
    }
    highest_avail = round_up_to_page(highest_avail);
    size_t num_bits = (highest_avail ) / PAGE_SIZE; //bit per page
    num_bits += 64; //so like at least we round up when we / sizeof int64

    physmem.end = highest_avail;
    physmem.mm_end = num_bits / (sizeof(uint64_t) * CHAR_BIT) - 1 ;
    for(int i = 0; i < physmem.mm_end; ++i)
        pmm[i] = UINT64_MAX;
    debugf("low addr 0x0000 high addr %lx in %li bits end = %li", highest_avail, num_bits, physmem.mm_end);

    for (int i = 0; i < mmap_size; ++i){
        multiboot_memory_map_t mmap = mmaps->entries[i];
        if(mmap.addr > physmem.end) break;
        
        phys_addr_t map_start = round_up_to_page(mmap.addr);
        phys_addr_t map_end = round_up_to_page(mmap.addr + mmap.len);

        for (phys_addr_t addr = map_start; addr < map_end; addr += PAGE_SIZE) {
            set_phys(addr, (mmap.type != MULTIBOOT_MEMORY_AVAILABLE || addr < PAGE_SIZE));
        }
    }
     for(size_t i = 0; i < (16); ++i)
        set_phys(PAGE_SIZE * i, 1); //ugh

    debugf("%i %i %i %i", get_phys(0xfffc0000), get_phys(0x100000000 + (PAGE_SIZE)), get_phys(0x100000000), get_phys(0xbffe0000 - PAGE_SIZE));

    bool dump = 0;
    if(dump){
        for(int i = 0; i < physmem.mm_end; ++i)
            debugf("[%d] %lb  {%lx - %lx}", i, pmm[i], (uint64_t)((uint64_t)PAGE_SIZE * i * 64), (uint64_t)((uint64_t)PAGE_SIZE * (uint64_t)i * (uint64_t)64) + ((uint64_t)PAGE_SIZE * 64));
    }
    
    return physmem.total_avail;
}


phys_addr_t phys_malloc(size_t size) {
    size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE; //up to nearest page ofc
    size_t contiguous_free_pages = 0;
    size_t start_idx = 0;
    size_t start_bit = 0;
    
    for (size_t idx = 0; idx < physmem.mm_end; ++idx) {
        uint64_t mask = pmm[idx];
        
        if (mask == UINT64_MAX) continue; // skip full blocks
        
        for (size_t bit = 0; bit < (sizeof(uint64_t) * CHAR_BIT); ++bit) {
            if ((mask & ((uint64_t)1 << bit)) == 0) { 
                if (contiguous_free_pages == 0) {
                    start_idx = idx;
                    start_bit = bit;
                }
                ++contiguous_free_pages;
                
                if (contiguous_free_pages == num_pages) {
                    // Mark the pages as allocated
                    for (size_t i = 0; i < num_pages; ++i) {
                        size_t alloc_idx = start_idx + (start_bit + i) / (sizeof(uint64_t) * CHAR_BIT);
                        size_t alloc_bit = (start_bit + i) % (sizeof(uint64_t) * CHAR_BIT);
                        pmm[alloc_idx] |= ((uint64_t)1 << alloc_bit);
                    }
                    debugf("phys_malloc(%lx) found %i ctp at %lx", size, contiguous_free_pages, (phys_addr_t)((start_idx * sizeof(uint64_t) * CHAR_BIT + start_bit) * PAGE_SIZE));
                    return (phys_addr_t)((start_idx * sizeof(uint64_t) * CHAR_BIT + start_bit) * PAGE_SIZE);
                }
            } else {
                contiguous_free_pages = 0; // encountered an allocated bit!
            }
        }
    }
    debugf("phys_malloc() failed!");
    return 0; // well shit
}

void phys_free(phys_addr_t addr, size_t size) 
{
    phys_addr_t f = addr;
    while(f < addr + size){
        set_phys(f, PHYS_FREE);
        f += PAGE_SIZE;
    }
}