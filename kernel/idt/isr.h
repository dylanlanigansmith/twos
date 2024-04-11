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

//64 

typedef void (*isr_t)(registers_t*);

void register_interupt_handler(uint8_t n, isr_t handler_fn); 