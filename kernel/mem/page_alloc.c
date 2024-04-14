#include "page_alloc.h"

#define MAX_TABLES (PALLOC_SIZE / sizeof(page_table_t)) //should be 1024

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
    table_index = 0;
    tables = tables_base = PALLOC_BASE;
}

page_table_t *palloc() //returns identity mapped address to an allocated page table
{
    ASSERT(table_index < MAX_TABLES - 1);

    //we should memset as well
    uintptr_t addr = table_top();

    memset(addr, 0, sizeof(page_table_t)); //so important

    page_table_t* ret = (page_table_t*)addr;

    table_index++;

    debugf("alloc new page table at %lx, index now %i \n", (uint64_t)ret, table_index);
    return ret;
}