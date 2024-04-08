#pragma once
#include "../stdlib/stdint.h"

#define PIC1_REMAP_OFFSET 0x20
#define PIC2_REMAP_OFFSET 0x28


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

//defines for pic remap contrl 
//https://wiki.osdev.org/PIC#Programming_the_PIC_chips
#define ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */
 

void PIC_init();

void PIC_remap(int off1, int off2);

void PIC_disable();
void PIC_enable();

void PIC_set_mask(uint8_t irq);
void PIC_clear_mask(uint8_t irq);