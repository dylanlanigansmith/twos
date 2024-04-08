#include "../drivers/video/video.h"
#include "types/string.h"
#include "../drivers/port/port.h"
#include "idt/idt.h"
#include "idt/isr.h"
#include "pic/pic.h"
#include "timer/timer.h"
#include "../drivers/input/keyboard.h"
#include "util/memory.h"
extern void panic();


void main()
{
   
    clear_screen();
   
    PIC_init();
    init_idt();
    //TODO: make a nicer c based GDT 

    //register isr
    keyboard_init();
    timer_init(10);
   
   
     __asm__ volatile ("sti");

     /*
     Issues:
     our stack is a hack
     bootloader sector reading is uh
     a wild guess that works
     */
    print("Kernel Main \n");
    //init IDT
    print("this is some random text so i know shit aint broke\n");
    print("you got no memory \n");
    print("you got a short stack \n"); 
    for(;;) __asm__ volatile("hlt;");

      
    // kernel ends, we can return to entry pt which hangs or just do it here for transparency while we develop
    __asm__("cli");
    for(;;){
        __asm__("hlt");
    } 
    __builtin_unreachable();
    
    //get keyboard driver working[x]
    //holy shit we can write a kernel sorta
    //get malloc
    //restart in proper gfx mode
    //see how ass fonts are
    // vga driver (this sounds fun)
    //ray caster

    //(get grub compat after standalone)

    //end result: sick project
    
}