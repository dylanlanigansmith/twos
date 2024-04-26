#include "stdio.h"
#include "string.h"
#include "os.h"

void print(const char* str){
     __asm__ volatile ("mov rdi, %0; mov rax, 4; int 0x69; " :: "r"(str) : "rdi", "rax") ;
}


void putc(char c){
    uint64_t ch = (uint64_t)c;
     __asm__ volatile ("mov rdi, %0; mov rax, 5; int 0x69; " :: "r"(ch) : "rdi", "rax") ;
}





void puts(const char *str)
{
         print(str); //lol
}



