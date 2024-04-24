#pragma once
#include "stdint.h"

#define PAGE_SIZE 0x200000 //2mib


static inline void* syscall(uint64_t num, void* arg){
     uint64_t ret = 0;
     __asm__ volatile ("mov rdi, %1; mov rax, %2; int 0x69; mov %0, rax; " : "=r"(ret) : "r"(arg), "r"(num) : "rdi","rax") ;
     return (void*)ret;
}


uint64_t sys_gettick();

static inline void sys_sleepms(uint64_t ms){
     __asm__ volatile ( "mov rax, 3; int 0x69;" :::  "rax") ;
}

void* set_gfx_mode(); 
void exit(uint64_t er );

void yield();

static inline void sys_shutdown(){
     __asm__ volatile (" mov rax, 12; int 0x69; " ::: "rdi", "rax") ;     
}

static inline uint64_t execc(const char* str){ //exec as child (blocks calling process)
     uint64_t ret = 0;
     uint64_t ptr = (uint64_t)str;
     __asm__ volatile ("mov rdi, %1; mov rax, 0xD; int 0x69; mov %0, rax; " : "=r"(ret) :  "r"(ptr) : "rdi", "rax") ;
    
     return ret; //and we come back like nothing ever happened
}

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define EXIT_ASSERTION 2
