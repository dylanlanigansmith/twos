#pragma once

#define ASSERT(condition) if( !( condition ) ) __on_fail_assert(#condition, __FILE__, __LINE__) 
#define assert(c) ASSERT(c)


static inline void __attribute__((noreturn)) __on_fail_assert(const char* cond, const char* file, int line){
     __asm__("mov rax, 0xdeadbeef; mov rdi, 0xdeadbeef; mov rbx, 0xdeadbeef; cli");
    for (;;)
        __asm__("hlt");
    __builtin_unreachable();
} 