#include "../drivers/video/console.h"
#include "stdlib.h"
#include "../drivers/port/port.h"
#include "idt/idt.h"
#include "idt/isr.h"
#include "pic/pic.h"
#include "timer/timer.h"
#include "../drivers/input/keyboard.h"
#include "../drivers/video/gfx.h"

extern void panic();


void main()
{
   /*
     Issues:
     our stack is a hack
     bootloader sector reading is uh
     a wild guess that works
     */
    clear_screen();
   
    PIC_init();
    init_idt();
    //TODO: make a nicer c based GDT & maybe multiboot
    //we can drop down to real mode again before init PIC/idts and all will be ok

    //register irq handlers
    keyboard_init();
    timer_init(10);
   
 
    enable_interupts();
    
    gfx_init(clr_white);

    gfx_print("kernel:\n");
    gfx_print("hey man \n"); // i was wondering if this was gonna work and if i might see the eclipse today or not thanks let me know");

     
    gfx_print("what what what");


    for(;;) __asm__ volatile("hlt;"); //wait for interupts

      
    
    
    //get keyboard driver working[x]
    //holy shit we can write a kernel sorta
    //get malloc [NAHHHHH]
    //restart in proper gfx mode
    //see how ass fonts are
 

    //video lib

    //ray caster

    //(get grub compat after standalone)

    //end result: sick project
    

    // kernel ends, we can return to entry pt which hangs or just do it here for transparency while we develop
    __asm__("cli");
    for(;;){
        __asm__("hlt");
    } 
    __builtin_unreachable();
}