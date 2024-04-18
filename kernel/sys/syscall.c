#include "syscall.h"

#include "../idt/isr.h"
#include "../task/task.h"
#define SYSCALL_TOTAL 3

void* syscall_test(registers_t* regs){
    print("your sis called ;)");

    return 0;
}


void* sys_print(registers_t* regs){
    if(regs->rdi != 0){
        print((char*)regs->rdi);
    }

    return 0;
}


void* sys_exit(registers_t* regs){
    exit(regs->rdi);

    return 0;
}



typedef void* (*syscall_fn)(registers_t* regs);

static void* syscalls[SYSCALL_TOTAL] =
{
    &syscall_test,
    &sys_print,
    &sys_exit,
};




static void handle_syscall(registers_t* regs){

    if(regs->rax >= SYSCALL_TOTAL) return;

    syscall_fn call = syscalls[regs->rax];

    //how do we want to handle args
    /*
        something like make macros for syscall wrappers

        parse args (rdi) into fmt
        idk
    */
    void* ret = call(regs);

    regs->rax = ret;



}

void syscall_init()
{
    register_interupt_handler(SYSCALL_INT, &handle_syscall);
}