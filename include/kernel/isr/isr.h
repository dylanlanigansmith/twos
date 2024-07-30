#pragma once
#include <kstdlib.h>

#define IRQ0 32
#define IRQ1 33
#define IRQ15 47



typedef struct {
    uint64_t ds;
   // uint64_t cr3;
    // Pushed by pusha macro
    uint64_t r15, r14, r13,r12,r11,r10,r9,r8;
    uint64_t rbp, rsi, rdi, rbx, rdx, rcx, cr3, rax;
   
    //pushed by us
    uint64_t int_no, err_code;
   
    
    // Pushed by the processor automatically.
    uint64_t  rip, cs, rflags, rsp, ss;
}  __attribute__((packed)) registers_t;


typedef void (*isr_t)(registers_t*);

void register_interupt_handler(uint8_t n, isr_t handler_fn); 