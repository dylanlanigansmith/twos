#pragma once
#include "../types/stdint.h"

//https://wiki.osdev.org/PIC#Programming_the_PIC_chips

#define PIC1		0x20		/* IO base address for master PIC */
#define PIC2		0xA0		/* IO base address for slave PIC */
#define PIC1_COMMAND	PIC1
#define PIC1_DATA	(PIC1+1)
#define PIC2_COMMAND	PIC2
#define PIC2_DATA	(PIC2+1)

#define PIC_EOI		0x20		/* End-of-interrupt command code */



#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */


void PIC_sendEOI(uint8_t irq);

uint16_t pic_get_irr();
uint16_t pic_get_isr();
