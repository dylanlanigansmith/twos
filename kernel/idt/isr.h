#pragma once
#include "../stdlib/stdint.h"
#define IRQ0 32
#define IRQ1 33
#define IRQ15 47

typedef struct {
    uint64_t ds;
    // Pushed by pusha.
    uint64_t edi, esi, ebp, ebx, edx, ecx, eax, int_no;
    uint64_t err_code, rsp; 
    //broskii theres r8-r15 too eek

    // Pushed by the processor automatically.
    uint64_t cs, eflags, eip, useresp, ss;
}  __attribute__((packed)) registers_t;

typedef struct {
    uint64_t ds;
    // Pushed by pusha macro
    uint64_t rdi, rsi, rbp, rbx, rdx, rcx, rax;
    uint64_t r8, r9, r10,r11,r12,r13,r14,r15;
    //pushed by us
    uint64_t int_no, err_code;
    //broskii theres r8-r15 too eek
    
    // Pushed by the processor automatically.
    uint64_t  rsp, cs, eflags, eip, useresp, ss;
}  __attribute__((packed)) registers64_t;
//64 

typedef void (*isr_t)(registers_t*);

void register_interupt_handler(uint8_t n, isr_t handler_fn); 