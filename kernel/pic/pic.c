#include "pic.h"
#include "../../drivers/port/port.h"
#include "../sys/sysinfo.h"

void PIC_sendEOI(uint8_t irq)
{
    if (irq >= 8)
        port_byte_out(PIC2_COMMAND, PIC_EOI);

    port_byte_out(PIC1_COMMAND, PIC_EOI);
}

static uint16_t __pic_get_irq_req(int ocw3) // ocw3 cmd word
{
    port_byte_out(PIC1_COMMAND, ocw3);
    port_byte_out(PIC2_COMMAND, ocw3);

    return (port_byte_in(PIC2_COMMAND) << 8) | port_byte_in(PIC1_COMMAND);
}

uint16_t pic_get_irr()
{
    return __pic_get_irq_req(PIC_READ_IRR);
}

uint16_t pic_get_isr()
{
    return __pic_get_irq_req(PIC_READ_ISR);
}

void PIC_init()
{
    // remap the PICs with offsets to no longer conflict with cpu irqs
    PIC_remap(PIC1_REMAP_OFFSET, PIC2_REMAP_OFFSET);
}

void PIC_remap(int off1, int off2)
{
    uint8_t a1, a2;
    if (sysinfo.boot.is_uefi)
    {
       // a1 = port_byte_in(PIC1_DATA); // save current masks
       // a2 = port_byte_in(PIC2_DATA);

        port_byte_out(PIC1_DATA, 0xff);
        port_byte_out(PIC2_DATA, 0xff);

        port_byte_out(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4); // init | icw4 = 0x11 to initialize
        io_wait();
        port_byte_out(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
        io_wait();

        port_byte_out(PIC1_DATA, off1); // master PIC vector offset
        io_wait();
        port_byte_out(PIC2_DATA, off2); // slave vec offset
        io_wait();

        port_byte_out(PIC1_DATA, 0x04); // master pic: there is a slave pic at irq2 (0b00000100)
        io_wait();
        port_byte_out(PIC2_DATA, 0x02); // tell slave its cascade identity (?) (0b0000010)
        io_wait();

        port_byte_out(PIC1_DATA, ICW4_8086); // use 8086 mode and not 8080
        io_wait();                           // this some history right here
        port_byte_out(PIC2_DATA, ICW4_8086);
        io_wait();

        port_byte_out(PIC1_DATA, 0); // restore masks we saved
        io_wait();
        port_byte_out(PIC2_DATA, 0);
        io_wait();

        PIC_clear_mask(0x20);
        PIC_clear_mask(0x21);
    }
    else
    {
        a1 = port_byte_in(PIC1_DATA); // save current masks
        a2 = port_byte_in(PIC2_DATA);
        port_byte_out(PIC1_COMMAND, ICW1_INIT + ICW1_ICW4); // init | icw4 = 0x11 to initialize
        io_wait();
        port_byte_out(PIC2_COMMAND, ICW1_INIT + ICW1_ICW4);
        io_wait();

        port_byte_out(PIC1_DATA, off1); // master PIC vector offset
        io_wait();
        port_byte_out(PIC2_DATA, off2); // slave vec offset
        io_wait();

        port_byte_out(PIC1_DATA, 0x04); // master pic: there is a slave pic at irq2 (0b00000100)
        io_wait();
        port_byte_out(PIC2_DATA, 0x02); // tell slave its cascade identity (?) (0b0000010)
        io_wait();

        port_byte_out(PIC1_DATA, ICW4_8086); // use 8086 mode and not 8080
        io_wait();                           // this some history right here
        port_byte_out(PIC2_DATA, ICW4_8086);
        io_wait();

        port_byte_out(PIC1_DATA, a1); // restore masks we saved
        port_byte_out(PIC2_DATA, a2);
    }
}

void PIC_disable()
{
    // mask all interupts, effectively disabling PICs
    port_byte_out(PIC1_DATA, 0xff);
    port_byte_out(PIC2_DATA, 0xff);
}
void PIC_enable()
{
    port_byte_out(PIC1_DATA, 0);
    port_byte_out(PIC2_DATA, 0);
}

void PIC_set_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = port_byte_in(port) | (1 << irq);
    port_byte_out(port, value);
}

void PIC_clear_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if (irq < 8)
        port = PIC1_DATA;
    else
    {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = port_byte_in(port) & ~(1 << irq);
    port_byte_out(port, value);
}
