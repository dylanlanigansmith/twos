#pragma once



#define offsetof(type, member)  __builtin_offsetof (type, member)

#define return_address(level) __builtin_return_address(level)

#define alloca(size) __builtin_alloca (size)
#define alloca_align(size, al)  __builtin_alloca_with_align(size, al) 


#define __noreturn __attribute__((noreturn))

//stolen from kernel, doing !! is really smart. 
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)


#define ___static_assert_str(c, str) _Static_assert(c,str)
#define ___static_assert_impl(c, str) ___static_assert_str(c, str)
#define static_assert(...) ___static_assert_impl(__VA_ARGS__, "Static Assertion Failed")
