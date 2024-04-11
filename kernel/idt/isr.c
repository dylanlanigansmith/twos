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


void dump_regs(registers_t regs)
{
    console_print(htoa(regs.ds));
    console_print(" ");
    console_print(htoa(regs.edi));
    console_print(" ");
    console_print(htoa(regs.esi));
    console_print(" ");
    console_print(htoa(regs.ebp));
    console_print(" ");
    console_print(htoa(regs.esp));
    console_print(" ");
    console_print(htoa(regs.ebx));
    console_print(" ");
    console_print(htoa(regs.edx));
    console_print(" ");
    console_print(htoa(regs.ecx));
    console_print(" ");
    console_print(htoa(regs.eax_idfk));
    console_print(" ");
    console_print(htoa(regs.int_no));
    console_print(" ");
    console_print(htoa(regs.err_code));
    console_print(" ");
    console_print(htoa(regs.eip));
    console_print(" ");
    console_print(htoa(regs.cs));
    console_print(" ");
    console_print(htoa(regs.eflags));
    console_print(" ");
    console_print(htoa(regs.useresp));
    console_print(" ");
    console_print(htoa(regs.ss));
    console_print(" ");
    console_print(htoa(regs.edi));
    console_print(" ");
}


#define EXCEPTION_PRINTI(str, num) serial_printi(str, num)
#define EXCEPTION_PRINTH(str, num) serial_printh(str, num)
#define EXCEPTION_PRINTLN(str) serial_println(str)
#define EXCEPTION_PRINTLN(str) println(str)
#define EXCEPTION_PRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
void handle_error_generic(int num, int err){
    EXCEPTION_PRINTLN("=======");
    if( 0 <= num && num < 32){
        EXCEPTION_PRINTI(exception_names[num], num);
        if(err){
            EXCEPTION_PRINTI("Non-Zero Error Code", 0);
        }
    }
    else{
         EXCEPTION_PRINTI("ISR_Unknown", num);
    }
    EXCEPTION_PRINTLN("=======");
}


int num_pfs = 0;
void handle_pagefault(registers_t* regs){
    uint64_t cr2 = 0;
    __asm__ volatile ("movq %0, cr2" : "=r" (cr2) );
    EXCEPTION_PRINTLN("== PAGE FAULT ==");
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

void isr_handler(uint64_t rdi, registers_t regs)
{ // really should get a pointer to stack where regs struct is instead of pass by ref
#ifdef DEBUG
    if (regs.err_code != 0)
        isr_log(regs.int_no, regs.err_code);

    else
        isr_log(regs.int_no, regs.err_code);
#endif

    switch (regs.int_no)
    {
    case ISR_PageFault:
        handle_pagefault(&regs);
        break;
    default: 
        handle_error_generic(regs.int_no, regs.err_code); break;

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
