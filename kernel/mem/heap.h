#pragma once
#include "../stdlib.h"
#define HEAP_PHYS 0x80000000
#define HEAP_VIRT 0xdeadbeef00000000
//dont fuckin ask
#define HEAP_VIRT_REAL 0xffffbeef00000000
#define HEAP_SIZE 0x800000

#define KMALLOC_FAIL 0

uintptr_t kmalloc(uint64_t size);

uintptr_t kfree(uint64_t address);

