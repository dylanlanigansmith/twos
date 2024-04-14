#pragma once

#define offsetof(type, member)  __builtin_offsetof (type, member)

#define return_address(level) __builtin_return_address(level)

#define alloca(size) __builtin_alloca (size)
#define alloca_align(size, al)  __builtin_alloca_with_align(size, al) 


#define likely(x, y) __builtin_expect(x,y)


//this isnt real
#define static_assert(c) __static_assert(c)