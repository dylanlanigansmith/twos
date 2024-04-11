#pragma once

#include "../stdlib.h"
typedef int PID_t;



typedef struct{
    PID_t pid;
    struct{
        uint64_t rsp, rbp; //stack
        uint64_t rip; //inst
        uint64_t rax, rbx, rcx, rdx, rsi, rdi;
        uint64_t r8, r9, r10,r11,r12,r13,r14,r15;
        uint64_t eflags, cr3;
    } regs; //so like uh can we talk about how we aint saving half of these with itrps
                //surely that wont be an issue

    void* vma;
    struct task_t* next;
} task_t;

int tasking_init();


int fork();


void yield(); 

PID_t getpid();