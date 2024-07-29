#include <kernel/panic.h>


void panic(const char* msg){
      __asm__("mov rax, 0xcafebabe; mov rdi, 0xcafebabe; mov rbx, 0xcafebabe; cli");
    for (;;)
        __asm__("hlt");
    __builtin_unreachable();
}