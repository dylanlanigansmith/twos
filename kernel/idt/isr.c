#include "isr.h"

#include "../stdlib/string.h"
#include "../stdlib/print.h"
#include "../../drivers/video/console.h"
#include "../pic/pic.h"
#include "../stdlib/memory.h"
#include "../../drivers/serial/serial.h"

#include "isr_exceptions.h"

isr_t interupt_handlers[256];





void print_isr_regs(registers_t* regs, bool extended)
{

    printf("===isr[%lx] err{%lx}====\n", regs->int_no, regs->err_code);
    printf("err_code = %lx\n", regs->err_code);
    printf("rbp = %lx\n", regs->rbp);
    printf("rip = %lx\n", regs->rip);
    printf("ss:rsp = %lx\n", regs->rsp);
    printf("cs = %lx\n", regs->cs);
    printf("rflags = %lx\n", regs->rflags);
    printf("ds = %lx\n", (uint64_t)(regs->ds & 0xffff));
    printf("ss = %lx\n", regs->ss);
    printf("rax = %lx\n", regs->rax);
    printf("rbx = %lx\n", regs->rbx);
    printf("rcx = %lx\n", regs->rcx);
    printf("rdx = %lx\n", regs->rdx);
    printf("rdi = %lx\n", regs->rdi);
    printf("rsi = %lx\n", regs->rsi);
    printf("r8 = %lx\n", regs->r8);
    printf("r9 = %lx\n", regs->r9);
    if(!extended) return;
    printf("r10 = %lx\n", regs->r10);
    printf("r11 = %lx\n", regs->r11);
    printf("r12 = %lx\n", regs->r12);
    printf("r13 = %lx\n", regs->r13);
    printf("r14 = %lx\n", regs->r14);
    printf("r15 = %lx\n", regs->r15);
}

#define EXCEPTION_PRINTI(str, num) serial_printi(str, num)
#define EXCEPTION_PRINTH(str, num) serial_printh(str, num)
#define EXCEPTION_PRINTLN(str) serial_println(str)
#define EXCEPTION_PRINTLN(str) println(str)
#define EXCEPTION_PRINTF(fmt, ...) printf(fmt, __VA_ARGS__)

int last_num = -1;

void handle_error_generic(registers_t* regs, bool recover){

    int num = regs->int_no; int err = regs->err_code; //lazy fuck

    EXCEPTION_PRINTLN("=======");
    if( 0 <= num && num < 32){
        EXCEPTION_PRINTI(exception_names[num], num);
        if(err){
            EXCEPTION_PRINTI("Non-Zero Error Code", 0);
        }
        print_isr_regs(regs, (num == 1) ? 1 : 0);
    }
    else{
         EXCEPTION_PRINTI("ISR_Unknown", num);
    }
    EXCEPTION_PRINTLN("=======");
    if(num == last_num || !recover)
        KPANIC("double exception! ");
    
    last_num = num;

}


int num_pfs = 0;
void handle_pagefault(registers_t* regs){

    //intel manual 3141 4.12
    uint64_t cr2 = 0;
    __asm__ volatile ("movq %0, cr2" : "=r" (cr2) );
    EXCEPTION_PRINTLN("\n== PAGE FAULT ==");
    EXCEPTION_PRINTF("Error: %lu \n", regs->err_code);
    EXCEPTION_PRINTF("At Address [CR2]: %lx \n", cr2);
    EXCEPTION_PRINTLN(  (regs->err_code & 0b100) ? "USER" : "KERNEL");
    EXCEPTION_PRINTLN(  (regs->err_code & 0b10) ? "WRITE" : "READ");
    EXCEPTION_PRINTLN(  (regs->err_code & 0b01) ? "PAGE-PROTECTION VIOLATION" : "PAGE NOT PRESENT");

    if(regs->err_code & 0b1000)    EXCEPTION_PRINTLN("CAUSE = RESERVED WRITE");
    if(regs->err_code & 0b10000)   EXCEPTION_PRINTLN("CAUSE = INSTRUCTION FETCH");
    if(regs->err_code & 0b100000)  EXCEPTION_PRINTLN("CAUSE = PROTECTION KEY VIOLATION");
    if(regs->err_code & 0b1000000) EXCEPTION_PRINTLN("CAUSE = SHADOW STACK ACCESS");

    EXCEPTION_PRINTLN("==============");

  //  if((regs->err_code & 0b1000) ) return;
    num_pfs++;
   
    if(num_pfs > 3){
        print_isr_regs(regs, 1);
        for(;;){
            __asm__("hlt");
        }
    }
        
   
}


int num_gpfs  = 0;
void handle_gpf(registers_t* regs){
    num_gpfs++;
    EXCEPTION_PRINTLN("\n=== GPF ===");
    EXCEPTION_PRINTF("Error: %lu \n", regs->err_code);
    EXCEPTION_PRINTF("rbp %lx \n", regs->rbp);
    EXCEPTION_PRINTLN("==============");
    //EXCEPTION_PRINTF("rsp %lx \n", regs->esp);
    print_isr_regs(regs, 1);
    if(num_gpfs > 1)
        for(;;){
            __asm__("hlt");
        }

}

void isr_handler(uint64_t rdi, registers_t regs)
{ // really should get a pointer to stack where regs struct is instead of pass by ref
#ifdef DEBUG
     print_isr_regs(&regs, 1); //we are frustrated and want these ASAP
#endif
    bool recoverable = True;
  
    if(regs.int_no == ISR_InvalidOpcode) recoverable = False;
    switch (regs.int_no)
    {
    case ISR_PageFault:
        handle_pagefault(&regs);
        break;
    case ISR_GeneralProtectionFault: //sorry for u bro
        handle_gpf(&regs);
        break;
    default: 
        handle_error_generic(&regs, recoverable); break;

    }
}

void irq_handler(uint64_t rdi, registers_t regs)
{
#ifdef DEBUG
    irq_log(regs.int_no, regs.err_code);
#endif
     PIC_sendEOI(regs.int_no); //dunno
    if (interupt_handlers[(uint8_t)(regs.int_no & 0xff)] != 0)
    {
        
        isr_t handler = interupt_handlers[regs.int_no];
        handler(&regs);
    }

   
}

void register_interupt_handler(uint8_t n, isr_t handler_fn)
{
    interupt_handlers[n] = handler_fn;

    debugf("registered interupt handler for [%i] at %lx\n", n, (uintptr_t)handler_fn);
 

}
