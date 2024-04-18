#pragma once

#include "../stdlib.h"

#include "../idt/isr.h"

typedef int PID_t;

typedef void (*task_entry_fn)();

#define MAX_TASK 128
#define ARBITRARY_SWITCH_INTERVAL 100
typedef struct{
        uint64_t rsp, rbp; //stack
        uint64_t rip; //inst
        uint64_t rax, rbx, rcx, rdx, rsi, rdi;
        uint64_t r8, r9, r10,r11,r12,r13,r14,r15;
        uint64_t rflags, cr3;
    } __attribute__((packed)) regs_t; 

typedef struct{
    char name[32];
    PID_t pid;
    //so like uh can we talk about how we aint saving half of these with itrps
                //surely that wont be an issue
    regs_t regs;
    uint64_t cr3;
    uint64_t reserved;

    struct {
        uint8_t no_swap : 1;
        uint8_t is_user : 1;
        uint8_t _zero : 6;
    } flags;

    struct task_t* next;
}__attribute__((packed)) task_t;



typedef struct{
    //task_t* tasks[MAX_TASK]; //we can just do this for now despite it being shitty 
                            //just go all in on linked list 
    task_t* current_task;   
    task_t* newest_task; //use this instead of tasks array! ^

    task_t* root_task;

    struct
    {
        uint64_t rflags;
        uint64_t cr3;
    }defaults;
    
    
    uint64_t timer;
    uint64_t next_switch;

    uint32_t tasks;
    PID_t pid_last;

    uint8_t active;
} scheduler_t;

extern scheduler_t sched;

int tasking_init(task_entry_fn main_fn);

int add_task(const char* name, task_entry_fn main_fn);

void start_first_task();

void on_timer_tick(uint64_t ticks, registers_t* reg);


void start_task(task_t* task);

//__attribute__((noreturn))
void  exit(int err);

void yield(); 

PID_t getpid();

