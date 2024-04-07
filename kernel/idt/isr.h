#pragma once
#include "../types/stdint.h"
#define IRQ0 32
#define IRQ15 47

typedef struct {
    uint32_t ds;
    // Pushed by pusha.
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    // Pushed by the processor automatically.
    uint32_t eip, cs, eflags, useresp, ss;
}  __attribute__((packed)) registers_t;

//64 

typedef void (*isr_t)(registers_t*);

void register_interupt_handler(uint8_t n, isr_t handler_fn); 