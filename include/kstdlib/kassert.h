#pragma once
#include <stdnoreturn.h>
#define ASSERT(condition) if( !( condition ) ) __on_fail_assert(#condition, __FILE__, __LINE__) 
#define assert(c) ASSERT(c)
void _assert_try_print(const char* str, const char* file, int line);

static inline void noreturn __on_fail_assert(const char* cond, const char* file, int line){
    _assert_try_print(cond, file, line);
     __asm__("mov rax, 0xdeadbeef; mov rdi, 0xdeadbeef; mov rbx, 0xdeadbeef; cli");
    for (;;)
        __asm__("hlt");
    __builtin_unreachable();
} 


