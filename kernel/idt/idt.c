#include "idt.h"
#include "../stdlib/memory.h"
#include "isr.h"
#include "../../drivers/video/console.h"
extern void* isr_stub_table[]; //in asm/idt.asm


idt_entry_t idt[256];
idtr_t idt_ptr;
extern isr_t interupt_handlers[];
extern void load_idt(uint64_t);

//https://web.archive.org/web/20190206105749/http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
//https://wiki.osdev.org/Interrupts_Tutorial

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags){
    idt_entry_t* descriptor = &idt[vector];


    
    descriptor->isr_low        = (uint16_t)( (uint64_t)isr & 0xFFFF);
    descriptor->kernel_cs      = (uint16_t)(0x8);
    descriptor->ist            = 0;
    descriptor->attributes     = flags;
    descriptor->isr_mid        = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high       = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved       = 0;

}

void init_idt(){
    idt_ptr.base = (uint64_t)&idt[0];
    idt_ptr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    memset(&idt, 0, sizeof(idt_entry_t) * 256 - 1);
    for(uint8_t vec = 0; vec < 48; ++vec){
        idt_set_descriptor(vec, isr_stub_table[vec], 0x8E);
    }

    typedef void (*isrfn)();
   
    load_idt((uint64_t)&idt_ptr);
   // __asm__ volatile ("lidt %0" : : "m"(idtr)); //load new idt 
    memset(&interupt_handlers, 0, sizeof(isr_t) * IDT_MAX_DESCRIPTORS - 1);
}