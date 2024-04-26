#include "os.h"



void exit(uint64_t er)
{
    __asm__ volatile("mov rax, 0x1; mov rdi, %0; int 0x69; " ::"r"(er) : "rdi", "rax");
}

void yield(){
     __asm__ volatile (" mov rax, 2; int 0x69") ;     

     __asm__ volatile (" nop; nop; nop; mov rax, rax; mov rdi, rdi; nop; nop; nop;") ;     
}

uint64_t sys_gettick(){
     uint64_t ret = 0;
     __asm__ volatile ( "mov rax, 10; int 0x69; mov %0, rax; " : "=r"(ret) ::  "rax") ;

     return ret;
}

/*
these are only in here bc i cant compile doom with masm-intel
-really should keep em inline and make a seperate att/gas version for the ones actually used in doom

*/