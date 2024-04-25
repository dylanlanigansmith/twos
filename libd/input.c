#include "input.h"

uint8_t keyboard_get_lastevent(){
     uint64_t ret = 0;
     __asm__ volatile ( "mov rax, 8; int 0x69; mov %0, rax; " : "=r"(ret) ::  "rax") ;

     return (uint8_t)(ret & 0xff);
}
uint8_t keyboard_popevent(){
     uint64_t ret = 0;
     __asm__ volatile ( "mov rax, 17; int 0x69; mov %0, rax; " : "=r"(ret) ::  "rax") ;

     return (uint8_t)(ret & 0xff);
}
 uint64_t keyboard_lastinput(){
     uint64_t ret = 0;
     __asm__ volatile ( "mov rax, 11; int 0x69; mov %0, rax; " : "=r"(ret) ::  "rax") ;

     return ret;
}

void keyboard_togglemode()
{
      __asm__ volatile ( "mov rax, 18; int 0x69;" ::: "rax") ;
}

uint8_t keyboard_is_down(uint8_t sc)
{
    uint64_t ret = 0;
    uint64_t s = (uint64_t)sc;
    __asm__ volatile("mov rdi, %1; mov rax, 9; int 0x69; mov %0, rax; " : "=r"(ret) : "r"(s) : "rax", "rdi");

    return (uint8_t)(ret & 0xff);
}