#pragma once
#include "page.h"
#include "heap.h"



#define PALLOC_SIZE (size_t)(PAGE_SIZE*PALLOC_SIZE_PAGES) //2mib = 512 page tables

#define MAX_TABLES (PALLOC_SIZE / sizeof(page_table_t)) //should be 1024 that is a lot (for now)
#define PALLOC_SIZE_PAGES 2 //2mib = 512 page tables







void palloc_init();

page_table_t* palloc();