#pragma once
#include "../stdlib.h"
#include "page.h"


uintptr_t user_palloc_init();

page_table_t * user_palloc();

uintptr_t make_user_virt_addr_space(size_t size, uintptr_t low_addr, uintptr_t high_addr);