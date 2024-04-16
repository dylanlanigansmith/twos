
#include "../kernel.h"
#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;




__attribute__((noreturn)) void __stack_chk_fail(void)
{
    const uint64_t fail_const = 0x11111111FA11ED11; 
    __asm__ volatile ("mov rax, %0 ; mov r15, rax ; mov r12, rax ; mov rdx, rax ;  int 1; " : : "m"(fail_const));; //maybe just  int 3 with funny regs
    panic("stack check fail");
    for(;;) {__asm__ volatile("nop");}

    __builtin_unreachable();
}
