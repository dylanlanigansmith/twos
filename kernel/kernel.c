#include "../drivers/video/video.h"
#include "types/string.h"
#include "../drivers/port/port.h"
#include "idt/idt.h"
#include "idt/isr.h"
#include "pic/pic.h"
#include "timer/timer.h"

extern void panic();

static void keydown_isr(registers_t* regs){

    uint8_t scan = port_byte_in(0x60);
    print("keydown");
    print(itoa(scan, 10));
    //lets fucking go
    
}
void main()
{
   
    clear_screen();
   
    PIC_init();
    init_idt();
    
    //register isr
    register_interupt_handler(0x21, (isr_t)keydown_isr);
    timer_init(10);
    //init pic and remap
     //PIC_enable();
    ///  __asm__ volatile ("cli");
   // port_byte_out(PIC1_DATA, 0xFE);
   // port_byte_out(PIC2_DATA, 0xFF);
   
     __asm__ volatile ("sti");
    print("Kernel Main \n");
    //init IDT
    print("this is some random text so i know shit aint broke\n");
    print("itrp");
  

   // __asm__ volatile("sti");

   
    for(;;) __asm__ volatile("hlt;");

      
    // kernel ends, we can return to entry pt which hangs or just do it here for transparency while we develop
    __asm__("cli");
    for(;;){
        __asm__("hlt");
    } 
    __builtin_unreachable();
    
    //get interupts working [x]
        //- get isrs working 
    //get keyboard driver working
    //holy shit we can write a kernel sorta
    //get malloc
    //restart in proper gfx mode
    //see how ass fonts are
    // vga driver (this sounds fun)
    //ray caster

    //(get grub compat after standalone)

    //end result: sick project
    
}