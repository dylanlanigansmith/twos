#include "isr.h"

#include "../stdlib/string.h"
#include "../stdlib/print.h"
#include "../../drivers/video/console.h"
#include "../pic/pic.h"
#include "../stdlib/memory.h"
#include "../../drivers/serial/serial.h"

#include "exception.h"

isr_t interupt_handlers[256];

//#define DEBUG
static inline void isr_log(uint32_t int_no, uint32_t err_code)
{
    // make hex
    uint16_t old_cursor = get_cursor();
    // set_cursor(50,CONSOLE_H - 1);
    serial_print("isr #[");
    serial_print(itoa(int_no, 10));
    serial_print("] err[");
    serial_print(htoa(err_code));
    serial_print("]\n");


    // set_cursor_offset(old_cursor);
}
static inline void irq_log(uint32_t int_no, uint32_t err_code)
{
    // make hex
    uint16_t old_cursor = get_cursor();
    set_cursor(0, CONSOLE_H - 1);
    console_print_str("irq #[", 0);
    console_print(itoa(int_no, 10));
    console_print("] err[");
    console_print(htoa(err_code));
    console_print("]\n");
    set_cursor_offset(old_cursor);
}




void print_isr_regs(registers_t* regs)
{
    printf("int_no = %lx\n", regs->int_no);
    printf("err_code = %lx\n", regs->err_code);
    printf("ds = %lx\n", regs->ds);
    printf("rdi = %lx\n", regs->edi);
    printf("rsi = %lx\n", regs->esi);
    printf("*rbp = %lx\n", regs->ebp);
    printf("rbx = %lx\n", regs->ebx);
    printf("rcx = %lx\n", regs->ecx);
    printf("rdx = %lx\n", regs->edx);
    printf("rax = %lx\n", regs->eax);
    printf("rsp = %lx\n", regs->rsp);
    printf("rip = %lx\n", regs->eip);
    printf("cs = %lx\n", regs->cs);
    printf("eflags = %lx\n", regs->eflags);
    printf("user_rsp = %lx\n", regs->useresp);
    printf("ss = %lx\n", regs->ss);
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
        print_isr_regs(regs);
    }
    else{
         EXCEPTION_PRINTI("ISR_Unknown", num);
    }
    EXCEPTION_PRINTLN("=======");
    if(num == last_num || !recover)
        panic("can't recover");
    
    last_num = num;

}


int num_pfs = 0;
void handle_pagefault(registers_t* regs){
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
    num_pfs++;
   
    if(num_pfs > 3)
        for(;;){
            __asm__("hlt");
        }
   
}


int num_gpfs  = 0;
void handle_gpf(registers_t* regs){
    num_gpfs++;
    EXCEPTION_PRINTLN("\n=== GPF ===");
    EXCEPTION_PRINTF("Error: %lu \n", regs->err_code);
    EXCEPTION_PRINTF("rbp %lx \n", regs->ebp);
    EXCEPTION_PRINTLN("==============");
    //EXCEPTION_PRINTF("rsp %lx \n", regs->esp);
    print_isr_regs(regs);
    if(num_gpfs > 2)
        for(;;){
            __asm__("hlt");
        }

}

void isr_handler(uint64_t rdi, registers_t regs)
{ // really should get a pointer to stack where regs struct is instead of pass by ref
#ifdef DEBUG
    if (regs.err_code != 0)
        isr_log(regs.int_no, regs.err_code);

    else
        isr_log(regs.int_no, regs.err_code);
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

    case ISR_Debug:
        print_isr_regs(&regs);
    default: 
        handle_error_generic(&regs, recoverable); break;

    }
}

void irq_handler(uint64_t rdi, registers_t regs)
{
#ifdef DEBUG
    irq_log(regs.int_no, regs.err_code);
#endif

    PIC_sendEOI(regs.int_no);
    if (interupt_handlers[(uint8_t)(regs.int_no & 0xff)] != 0)
    {
        isr_t handler = interupt_handlers[regs.int_no];
        handler(&regs);
    }
}

void register_interupt_handler(uint8_t n, isr_t handler_fn)
{
    interupt_handlers[n] = handler_fn;
#ifdef DEBUG
    print("registered interupt handler for [");
    print(itoa(n, 10));
    print("]\n");
#endif
}
