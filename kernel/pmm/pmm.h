#pragma once
#include "../stdlib.h"
#define BYTES_TO_KIB(bytes) (bytes / 1024llu)
#define BYTES_TO_MIB(bytes) (bytes / 1048576llu)
#define BYTES_TO_GIB(bytes) (bytes / 1073741824llu)

#define ONE_GIB 0x40000000llu
#define ONE_MIB 0x100000llu
              //0x100000

#define USER_PHYS_START ONE_GIB

void pmm_init();

uintptr_t pmm_kalloc(size_t size);

uintptr_t pmm_alloc(size_t size);

uintptr_t pmm_mark_frames_used(uintptr_t addr, size_t size);

void pmm_debug_space();