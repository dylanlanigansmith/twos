#pragma once

#include "../stdlib.h"

#include "../idt/isr.h"
#include "elf.h"
typedef int PID_t;

typedef void (*task_entry_fn)();

#define MAX_TASK 128
#define ARBITRARY_SWITCH_INTERVAL 10

/*
so if pit rate = 1000hz
1 tick = 1 ms (except qemu pit bad)
100ms per task
*/
#define TASK_ORPHAN 0xFFFF
typedef struct{
        uint64_t rsp, rbp; //stack
        uint64_t rip; //inst
        uint64_t rax, rbx, rcx, rdx, rsi, rdi;
        uint64_t r8, r9, r10,r11,r12,r13,r14,r15;
        uint64_t rflags, cr3;
    } __attribute__((packed)) regs_t; 


typedef struct task_t task_t;

typedef struct task_t{
    char name[32];
    PID_t pid;
    PID_t parent_PID;
    regs_t regs;
    uint64_t _idk;
    union{
        task_t* parent_task;
        uint64_t wake_tick;
    };
    struct {
        uintptr_t heap_virt;
        uintptr_t heap_phys;
        size_t heap_size;
    }maps;    
    void* vma; 
    uint64_t cs, ds;
    struct {
        uint8_t no_swap : 1;
        uint8_t is_user : 1;
        uint8_t blocks_parent : 1;
        uint8_t sleeping : 1;
    } flags;
    void* mem;
    task_t* next;
}__attribute__((packed)) task_t;



typedef struct{
    //task_t* tasks[MAX_TASK]; //we can just do this for now despite it being shitty 
                            //just go all in on linked list 
    task_t* current_task;   
    task_t* newest_task; //use this instead of tasks array! ^

    task_t* root_task;
    bool skip_saving_regs;

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

int add_user_task(const char* name, user_vas_t* usr);

void start_first_task();

void on_timer_tick(uint64_t ticks, registers_t* reg);


void start_task(task_t* task);

//__attribute__((noreturn))
void  exit(int err);

void yield(); 

PID_t getpid();

void sleep_ns(uint64_t ns);

int exec_user_task(const char* taskname, registers_t* regs);


void* task_alloc_heap(size_t size);

void* task_mmap_file(const char* name);

static inline uint64_t rdtsc() //idk where else to put this
{
    uint32_t low, high;
    asm volatile("rdtsc":"=a"(low),"=d"(high));
    return ((uint64_t)high << 32) | low;
}