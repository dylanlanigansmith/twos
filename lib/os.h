#pragma once
#include "shared.h"

static inline void* syscall(uint64_t num, void* arg){
     __asm__ volatile ("mov rdi, %0; mov rax, %1; int 0x69; " :: "r"(arg), "r"(num) : "rdi","rax") ;
}


static inline void* print(const char* str){
     __asm__ volatile ("mov rdi, %0; mov rax, 1; int 0x69; " :: "r"(str) : "rdi", "rax") ;
}