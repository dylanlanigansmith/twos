#include "vas.h"
#include "../pmm/pmm.h"

#include "page.h"

uint64_t pt_base;
size_t  pt_index;

#define MAX_USER_PAGES 1024
uintptr_t user_palloc_init()
{
    pt_base = pt_index = 0;

    pt_base = pmm_alloc(PAGE_SIZE * 2); //1024 user pages

    if(!pt_base) KPANIC("couldn't alloc space for user pages");

    ASSERT(map_phys_addr(pt_base, pt_base, PAGE_SIZE * 2, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER));
}



page_table_t *user_palloc() //returns physical address of a new page table
{
    ASSERT(pt_index < MAX_USER_PAGES - 1);

    
    uintptr_t addr = (uintptr_t)pt_base + sizeof(page_table_t) * pt_index;

    

    memset((void*)addr, 0, sizeof(page_table_t)); 

    page_table_t* ret = (page_table_t*)addr;

    pt_index++;

    debugf("USER palloc new page table at %lx, index now %i \n", (uint64_t)ret, pt_index);
    return ret;
}


uintptr_t make_user_virt_addr_space(size_t size, uintptr_t low_addr, uintptr_t high_addr)
{


    return 0;
}
