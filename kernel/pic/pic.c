#include "pic.h"
#include "../../drivers/port/port.h"


void PIC_sendEOI(uint8_t irq)
{
	if(irq >= 8)
		port_byte_out(PIC2_COMMAND,PIC_EOI);
 
	port_byte_out(PIC1_COMMAND,PIC_EOI);
}


static uint16_t __pic_get_irq_req(int ocw3) //ocw3 cmd word
{
    port_byte_out(PIC1_COMMAND, ocw3);
    port_byte_out(PIC2_COMMAND, ocw3);

    return (port_byte_in(PIC2_COMMAND) << 8) | port_byte_in(PIC1_COMMAND);
}

uint16_t pic_get_irr(){
    return __pic_get_irq_req(PIC_READ_IRR);
}

uint16_t pic_get_isr(){
    return __pic_get_irq_req(PIC_READ_ISR);
}