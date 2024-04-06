#include "idt.h"
#include "../util/memory.h"

extern void idt_flush(uint32_t);


idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;
//https://web.archive.org/web/20190206105749/http://www.jamesmolloy.co.uk/tutorial_html/4.-The%20GDT%20and%20IDT.html
//https://wiki.osdev.org/Interrupts_Tutorial

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags){

}

static void init_idt(){

}