#include "timer.h"

#include "../idt/isr.h"
#include "../../drivers/video/console.h"
#include "../../drivers/port/port.h"

uint32_t tick = 0;

static void timer_handler(registers_t* reg){
    tick++;
    uint16_t old_cursor = get_cursor();
   
   #ifdef VGA_MODE_CHAR
    set_cursor(0,CONSOLE_H - 2);
    console_print(itoa(tick, 10));
    set_cursor_offset(old_cursor);
    #endif
}
void timer_init(uint32_t freq)
{
    register_interupt_handler(IRQ0, &timer_handler);
    uint32_t freq_divide = TIMER_CLOCK_IN / freq;
    port_byte_out(TIMER_CMD, TIMER_MODE_REPEAT); //set to repeat mode, and that next we will send divisor

    uint8_t l = (uint8_t)(freq_divide & 0xff);
    uint8_t h = (uint8_t)((freq_divide >> 8) & 0xff); //gotta send divisor byte by byte so split it up

    port_byte_out(TIMER_REG0, l);
    port_byte_out(TIMER_REG0, h);
}
