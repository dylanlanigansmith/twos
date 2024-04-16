#pragma once
/*
Common Header for our shared functions, stdlib, etc

we dont miss libc at all!
*/

#include "stdlib/stdint.h"
#include "stdlib/string.h"
#include "stdlib/memory.h"
#include "stdlib/stdbool.h"
#include "stdlib/assert.h"
#include "stdlib/print.h"
#include "stdlib/macro.h"
#include "stdlib/binary.h"


static_assert(sizeof(void*) == sizeof(uintptr_t) && sizeof(uintptr_t) == sizeof(uint64_t) && sizeof(uint64_t) == 8UL, "longs ain't long enough!");
static_assert(sizeof(uint8_t) == 1, "bytes dont bite!");
static_assert(sizeof(uint16_t) == 2, "shorts aint short enough");
static_assert(sizeof(uint32_t) == 4, "yeah if you are seeing this good f-ing luck");