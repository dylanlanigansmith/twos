#include "os.h"

void exit(uint64_t er ){ 
     __asm__ volatile ("mov rax, 0x1; mov rdi, %0; int 0x69; " :: "r"(er) : "rdi", "rax");     
}