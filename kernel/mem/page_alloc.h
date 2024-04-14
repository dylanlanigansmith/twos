#pragma once
#include "page.h"
#include "heap.h"

#define PALLOC_BASE (0x50000000 )

#define PALLOC_SIZE_PAGES 2 //2mib = 512 page tables
#define PALLOC_SIZE (size_t)(PAGE_SIZE*PALLOC_SIZE_PAGES) //2mib = 512 page tables




void palloc_init();

page_table_t* palloc();