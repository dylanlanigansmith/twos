#include "task.h"
#include "../mem/heap.h"
#include "../../drivers/video/gfx.h"
#define DEBUGT(fmt, ...) printf(fmt, __VA_ARGS__)

extern void capture_regs(regs_t* regs);
extern void restore_regs(regs_t* regs);
extern void fix_isr_regs();
PID_t pid_last = 0;
task_t* task_current;

extern uint64_t get_cr3();


void print_regs(regs_t* regs)
{
    println("=======");
    printf("rsp: %lx\n", regs->rsp);
    printf("rbp: %lx\n", regs->rbp);
    printf("rip: %lx\n", regs->rip);
    printf("rax: %lx\n", regs->rax);
    printf("rbx: %lx\n", regs->rbx);
    printf("rcx: %lx\n", regs->rcx);
    printf("rdx: %lx\n", regs->rdx);
    printf("rsi: %lx\n", regs->rsi);
    printf("rdi: %lx\n", regs->rdi);
    printf("r8: %lx\n", regs->r8);
    printf("r9: %lx\n", regs->r9);
    printf("r10: %lx\n", regs->r10);
    printf("r11: %lx\n", regs->r11);
    printf("r12: %lx\n", regs->r12);
    printf("r13: %lx\n", regs->r13);
    printf("r14: %lx\n", regs->r14);
    printf("r15: %lx\n", regs->r15);
    printf("rflags: %lx\n", regs->rflags);
    printf("cr3: %lx\n", regs->cr3);
    println("=======");
}


void t1_main(){
sleep:
    uint64_t j = 1;
    for(int i = 0; i < 10000000; ++i){
        j *= 2;
    }
    printf("task1: i did a task %li\n\n\n\n\n", j);

   
    yield();

    println("nice try");
    goto sleep;
}


task_t main_task, t1;
void create_task(task_t* task, task_entry_fn main_fn, uint64_t rflags, uint64_t cr3, uint64_t rsp, uint64_t rbp) //ignore flags and vma for now 
{
    task->pid = pid_last++;
    
    // Initialize regs_t members to 0
    uint64_t new_stack = kmalloc(4096 * 4) + 4096 * 4;

    task->regs.rsp = new_stack;
    task->regs.rbp = new_stack;
    task->regs.rip = (uint64_t)main_fn;
    task->regs.rax = 0;
    task->regs.rbx = 0;
    task->regs.rcx = 0;
    task->regs.rdx = 0;
    task->regs.rsi = 0;
    task->regs.rdi = 0;
    task->regs.r8 = 0;
    task->regs.r9 = 0;
    task->regs.r10 = 0;
    task->regs.r11 = 0;
    task->regs.r12 = 0;
    task->regs.r13 = 0;
    task->regs.r14 = 0;
    task->regs.r15 = 0;
    task->regs.rflags = rflags;
    task->regs.cr3 = cr3;

    // Initialize vma and next to NULL
    task->reserved = nullptr;
    task->next = nullptr;
}

void test(){
    print("yes!");
}

int tasking_init(task_entry_fn main_fn){


    DEBUGT("%s", "tasking init");

    regs_t r; capture_regs(&r); //for eflags and cr3 initial
   //  main_task.regs.rip = &test;
   // restore_regs(&r);
    main_task.regs = r;
    main_task.regs.rip = main_fn;
   
    create_task(&t1, &t1_main, main_task.regs.rflags, main_task.regs.cr3, main_task.regs.rsp, main_task.regs.rbp);
    main_task.next = &t1;
   
    t1.next = &main_task;
    task_current = &main_task;
    return 0;
}


void switchTask(task_t* prev, task_t* new){
    //yk i love that you can use new as a name in c
    uint64_t return_address =  __builtin_return_address (1);
    printf("return address: %lx", return_address);
    __asm__ volatile ("cli");
    capture_regs(&prev->regs);
   // print_regs(&new->regs);
    prev->regs.rip = return_address;
   //  main_task.regs.rip = &test;
    restore_regs(&(new->regs));
    __asm__ volatile ("sti");
    //no fuckin way
}
bool first = False;
void yield(){
    
    
    task_t* prev = task_current;
    ASSERT(prev != nullptr);

    task_current = task_current->next;
   // switchTask(prev, task_current);
   //yk i love that you can use new as a name in c
    uint64_t return_address =  __builtin_return_address (0);
   // if(first) gfx_clear(color_red);
   // printf("return address: %lx", return_address);
    __asm__ volatile ("cli");
    capture_regs(&prev->regs);
    prev->regs.rip = return_address;
    
   // print_regs(&prev->regs);
   // print_regs(&task_current->regs);
   // if(first) panic("hm");
    first = True;
   //  main_task.regs.rip = &test;
    restore_regs(&task_current->regs);
    
    __asm__ volatile ("sti");
    
}

PID_t getpid(){
    return task_current->pid;
}

int fork(){
    return 0;
}
