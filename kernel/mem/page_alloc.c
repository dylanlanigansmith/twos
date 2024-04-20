#include "page_alloc.h"
#include "../pmm/pmm.h"



#define PALLOC_BASE (0x50000000 )
uintptr_t tables_base; //base

page_table_t (*tables)[MAX_TABLES]; //a pointer to an array of page_tables
                //its easier to just keep it as a raw pointer but this is actually whats goin on type wise/structure wise
//we can literally have a one byte array sorta thing for impl palloc_free() in future
// and just mark when used ezpz




size_t table_index;

uintptr_t table_top(){ //get top of free space (where new table can go)
    return (uintptr_t)tables_base + sizeof(page_table_t) * table_index;
}

void palloc_init()
{
    table_index =tables = tables_base = 0;
    //plead with pmm for some memory 
    tables_base = pmm_kalloc(PALLOC_SIZE) + KERNEL_ADDR;
    if(!tables_base){
        //well shit this is really bad
        debugf("palloc init failed, no physical memory"); KPANIC("Can't Init PALLOC. We're fucked");
    }
    ASSERT(map_phys_addr(tables_base, tables_base - KERNEL_ADDR, PALLOC_SIZE, 0));
    debugf("page allocator ready\n");
}

page_table_t *palloc() //returns identity mapped address to an allocated page table
{
    ASSERT(table_index < MAX_TABLES - 1);

    //we should memset as well
    uintptr_t addr = table_top();

    memset(addr, 0, sizeof(page_table_t)); //so important

    page_table_t* ret = (page_table_t*)addr - KERNEL_ADDR;

    table_index++;

    debugf("alloc new page table at %lx, index now %i \n", (uint64_t)ret, table_index);
    return ret;
}