#pragma once
//the things i do for red squiggles
#ifdef __INTELLISENSE__
    #include <stdint-gcc.h>
    #define nullptr ((void*)0) 
    #define true ((bool)1u)
    #define false ((bool)0u)
    // ^ c23 support sorta broken
#else
    #include <stdint.h> //intellisense didnt like this 
#endif

#include <stdarg.h>
#include <limits.h>
#include <stdbool.h>
#include <float.h>
#include <stddef.h>
#include <stdnoreturn.h>

#include "kassert.h"




#ifndef offsetof
    #define offsetof(type, member)  __builtin_offsetof (type, member)
#endif 

#define return_address(level) __builtin_return_address(level)

#define alloca(size) __builtin_alloca (size)
#define alloca_align(size, al)  __builtin_alloca_with_align(size, al) 




//stolen from kernel, doing !! is really smart.
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)


#define static_assert(c, str) _Static_assert(c,str)

//typedef uint64_t size_t;
static_assert(sizeof(size_t) == 8UL, "size_t missing or wrong size");
static_assert(sizeof(void*) == sizeof(uintptr_t) && sizeof(uintptr_t) == sizeof(uint64_t) && sizeof(uint64_t) == 8UL, "longs ain't long enough!");
static_assert(sizeof(uint8_t) == 1, "bytes dont bite!");
static_assert(sizeof(uint16_t) == 2, "shorts aint short enough");
static_assert(sizeof(uint32_t) == 4, "yeah if you are seeing this good f-ing luck");
static_assert(sizeof(bool) == 1, "bools are fools");

static_assert(nullptr == 0, "nullptr isnt null, just fuckin give up man");


#define BYTES_TO_KIB(bytes) (bytes / 1024llu)
#define BYTES_TO_MIB(bytes) (bytes / 1048576llu)
#define BYTES_TO_GIB(bytes) (bytes / 1073741824llu)

#define ONE_GIB 0x40000000llu
#define ONE_MIB 0x100000llu

#define PACKED  __attribute__((packed)) 


#define VM_ONLY 1
#define VM_QEMU "QEMU"
