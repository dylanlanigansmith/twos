#pragma once
#include "../stdlib.h"

              //0x100000

#define USER_PHYS_START ONE_GIB

void pmm_init();

uintptr_t pmm_kalloc(size_t size);

uintptr_t pmm_alloc(size_t size);

uintptr_t pmm_mark_frames_used(uintptr_t addr, size_t size);

void pmm_debug_space();