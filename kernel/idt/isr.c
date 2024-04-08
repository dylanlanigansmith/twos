#include "isr.h"

#include "../stdlib/string.h"
#include "../../drivers/video/console.h"
#include "../pic/pic.h"
#include "../stdlib/memory.h"

isr_t interupt_handlers[256];

//#define DEBUG
static inline void isr_log(uint32_t int_no, uint32_t err_code){
    //make hex
    uint16_t old_cursor = get_cursor();
   // set_cursor(50,CONSOLE_H - 1);
    console_print_str("isr #[", 0);
    console_print(itoa(int_no, 10));
    console_print("] err[");
    console_print(itoa(err_code, 10));
    console_print("]\n");
   // set_cursor_offset(old_cursor);
}
static inline void irq_log(uint32_t int_no, uint32_t err_code){
    //make hex
    uint16_t old_cursor = get_cursor();
    set_cursor(0,CONSOLE_H - 1);
    console_print_str("irq #[", 0);
    console_print(itoa(int_no, 10));
    console_print("] err[");
    console_print(itoa(err_code, 10));
    console_print("]\n");
    set_cursor_offset(old_cursor);
}


void isr_handler(registers_t regs){ //really should get a pointer to stack where regs struct is instead of pass by ref
   

    if(regs.err_code != 0)
        isr_log(regs.int_no, regs.err_code);
    #ifdef DEBUG
    else
         isr_log(regs.int_no, regs.err_code);
    #endif


    switch(regs.int_no)
    {
        case 6:
            //for(;;) { __asm__ volatile("hlt");}

    }
}
  


void irq_handler(registers_t regs) {
     #ifdef DEBUG
         irq_log(regs.int_no, regs.err_code);
    #endif
    
    PIC_sendEOI(regs.int_no);
    if(interupt_handlers[(uint8_t)(regs.int_no & 0xff)] != 0){
        isr_t handler = interupt_handlers[regs.int_no];
        handler(&regs);
    }
   
    
}


void register_interupt_handler(uint8_t n, isr_t handler_fn){
    interupt_handlers[n] = handler_fn;
     #ifdef DEBUG
        print("registered interupt handler for [");
        print(itoa(n, 10));
        print("]\n");
    #endif
}
