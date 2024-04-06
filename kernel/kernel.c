#include "../drivers/video/video.h"
#include "types/string.h"
#include "../drivers/port/port.h"
#include "idt/idt.h"
extern void panic();

void dbg_cursor(){
    int x = get_cursor_x();
    int y = get_cursor_y();
    print(itoa(x, 10));
    print(" ");
    print(itoa(y, 10));
}
void main()
{
   
    //init IDT

    init_idt();

    //init pic

    //register isr

    //bada bing


    clear_screen();
    set_cursor(0,0);
    print("dude please work \n");
    print("it works wow\n");
    print("this is so fun!\n");
    print("words on a screen\n");
    
    //print(itoa(100 / 0, 10));
   

   print_str("so like the other day i was totally writing this sick os, \n and it printed this string without any issues", 0);
    
    
    
    for(;;){} 
    __builtin_unreachable();
    
    //get interupts working
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