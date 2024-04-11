#pragma once
#include "../stdlib/stdint.h"
#define IRQ0 32
#define IRQ1 33
#define IRQ15 47

typedef struct {
    uint64_t ds;
    // Pushed by pusha.
    uint64_t edi, esi, ebp, esp, ebx, edx, ecx, int_no;
    uint64_t err_code, eax_idfk; 
    //broskii theres r8-r15 too eek
    // Pushed by the processor automatically.
    uint64_t eip, cs, eflags, useresp, ss;
}  __attribute__((packed)) registers_t;

//64 

typedef void (*isr_t)(registers_t*);

void register_interupt_handler(uint8_t n, isr_t handler_fn); 