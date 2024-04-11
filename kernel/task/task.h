#pragma once

#include "../stdlib.h"
typedef int PID_t;

typedef void (*task_entry_fn)();

typedef struct{
        uint64_t rsp, rbp; //stack
        uint64_t rip; //inst
        uint64_t rax, rbx, rcx, rdx, rsi, rdi;
        uint64_t r8, r9, r10,r11,r12,r13,r14,r15;
        uint64_t rflags, cr3;
    } __attribute__((packed)) regs_t; 

typedef struct{
    PID_t pid;
    //so like uh can we talk about how we aint saving half of these with itrps
                //surely that wont be an issue
    regs_t regs;
    uint64_t cr3;
    uint64_t reserved;
    struct task_t* next;
}__attribute__((packed)) task_t;

int tasking_init(task_entry_fn main_fn);


int fork();


void yield(); 

PID_t getpid();