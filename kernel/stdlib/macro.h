#pragma once

#define offsetof(type, member)  __builtin_offsetof (type, member)

#define return_address(level) __builtin_return_address(level)

#define alloca(size) __builtin_alloca (size)
#define alloca_align(size, al)  __builtin_alloca_with_align(size, al) 




//stolen from kernel, doing !! is really smart.
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

//this isnt real
#define static_assert(c, str) _Static_assert(c,str)