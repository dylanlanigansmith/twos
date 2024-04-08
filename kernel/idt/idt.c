#include "idt.h"
#include "../stdlib/memory.h"
#include "isr.h"
extern void* isr_stub_table[]; //in asm/idt.asm


idt_entry_t idt[256];
static idtr_t idtr;
extern isr_t interupt_handlers[];
extern void load_idt(uint32_t);

//https://web.archive.org/web/20190206105749/http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
//https://wiki.osdev.org/Interrupts_Tutorial

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags){
    idt_entry_t* descriptor = &idt[vector];


    descriptor->isr_low = (uint32_t)isr & 0xFFFF; 
    descriptor->kernel_cs = 0x8; // kernel code selector in gdt
    descriptor->attributes = flags;
    descriptor->isr_high = (uint32_t)isr >> 16;
    descriptor->reserved = 0;
}

void init_idt(){
    idtr.base = (uint32_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

    memset(&idt, 0, sizeof(idt_entry_t) * 256 - 1);
    for(uint8_t vec = 0; vec < 48; ++vec){
        idt_set_descriptor(vec, isr_stub_table[vec], 0x8E);
    }
    //okay lets try it
    load_idt((uint32_t)&idtr);
   // __asm__ volatile ("lidt %0" : : "m"(idtr)); //load new idt 


    //__asm__ volatile ("sti"); //set intrp flag

    memset(&interupt_handlers, 0, sizeof(isr_t) * IDT_MAX_DESCRIPTORS - 1);
}