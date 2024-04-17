#include "../drivers/video/console.h"
#include "stdlib.h"
#include "../drivers/port/port.h"
#include "idt/idt.h"
#include "idt/isr.h"
#include "pic/pic.h"
#include "timer/timer.h"
#include "../drivers/input/keyboard.h"
#include "../drivers/video/gfx.h"
#include "../drivers/video/vbe.h"

#include "../drivers/sound/sound.h"
#include "boot/multiboot2.h"
#include "boot/mb_header.h"

#include "sys/sysinfo.h"
#include "../drivers/serial/serial.h"

#include "mem/page.h"
#include "mem/heap.h"

#include "task/task.h"
#include "stdio/stdout.h"
#include "gdt/gdt.h"

#include "mem/page_alloc.h"

#include "acpi/madt.h"

#include "fs/initrd.h"

void user_mode_test(){
    __asm__ volatile("mov rdi, 0xcafebabe; mov rax, rdi; mov r12, rdi; mov rdx, rdi;  int 0x0; ");
    for(;;){
        __asm__("hlt");
    }
}

int cpp_test(int, int);

void wtf(){
     __asm__ volatile ( 
        
        " \
        nop; nop; xor rax, rax; nop; \
        int 3; \
        hlt;   "

    );
}


uint64_t start_tick = 0;

bool first_call = False;

uint64_t last_tick = 0;
void task_drawtimer(){
    
    serial_print("task draw timer \n");
     gfx_clear_line(598, 24);
    for(;;){
        if((last_tick + 100) < tick  ){
             //   serial_printi("t=", tick);
                 gfx_clear_line(598, 24);
                last_tick = tick;
                gfx_print_pos(lltoa(tick, 10), v2(5, 600));
            }
    }
}
uint64_t last_tick2 = 0;
void task_draw_test(){
    
    serial_print("task draw test \n");
     gfx_clear_line(498, 24);
    for(;;){
        __asm__("hlt");
        if((last_tick2 + 100) < tick  ){

            if(stdout_dirty()){
                 gfx_clear_text();
                 __asm__ volatile ("cli");
                  gfx_print(get_stdout());
                  stdout_flush();
                  __asm__ volatile ("sti");
            }
             
         //   gfx_clear(gfx_state.clear_color);
           
             //   serial_printi("t=", tick);
              //  gfx_clear_line(498, 24);
               // last_tick2 = tick;
                //gfx_print_pos(lltoa(tick, 10), v2(5, 500));
            }
    }
}

extern uint64_t has_cpuid();

extern __attribute__((noreturn)) void jump_to_usermode(void* addr);


void main(void *addr, void *magic)
{
    if ((uint64_t)magic != MULTIBOOT2_BOOTLOADER_MAGIC){ /* uh how? */ }
    //we do check this in parse_multiboot_header() but that does happen rather late in the start process 
    disable_interupts();
   
    serial_init(); //we can now debug and log! 
    debug("boot start");
    
    if(port_byte_in(0xe9) == 0xe9){
        serial_set_e9();
        debug("Using Bochs E9 Hack for debug out\n"); //bochs
    } else{
        debug("using Serial COM1 for debug out \n"); //qemu (superior)
    } //todo: https://wiki.osdev.org/Parallel_port and use that for kernel debug
    
    //should check if we can do this sooner!



    //should check multiboot here while we have a working system

    init_descriptor_table(2); //update gdt
                            //add user mode segments 
                            //add tss
   

    //todo: APIC 
    PIC_init();
   
    //our old friend
    init_idt();
    
    // register irq handlers
    keyboard_init();
    timer_init(PIT_RATE);

    
   

    io_wait();
    serial_println("boot phase 1 complete"); 
   // serial_println("boot phase 1 complete"); 
   // serial_println("boot phase 1 complete"); 
   // serial_println("boot phase 1 complete"); 

    
    debugf("multiboot2 addr = %lx magic = %lx \n",  (uintptr_t)addr, (uint64_t)magic);
    

    //so we gotta do this sooner in boot stage! 
    if(parse_multiboot_header(addr, (uint64_t)magic) == MB_HEADER_PARSE_ERROR){ //gets acpi, framebuffer, etc

        KPANIC("multiboot parse fail");
    }else{
        //do shit like are we uefi, what vm is this etc etc etc 
      
    }
    
    
    debugf("CPUID available = %s\n", (has_cpuid()) ? "YES" : "NO WTF");
   
    // serial_println("\n===init mem===\n");
    make_page_struct(); //this also initializes heap, maps frame buffer
    //doing this pre-enable interupts now to see if stability improves 
    _init_cpp(); //bc we cant get global constructors to work & by we i mean I
    

    if(sysinfo.rsdp){  

        uintptr_t rsdt = map_phys_addr(ACPI_ADDR, sysinfo.rsdp->RsdtAddress, PAGE_SIZE * 2, 0b10000001LLU ) ;
                                        // base mapping and offset from it 
        ACPI_discover_SDTs( (void*)(rsdt), ACPI_ADDR, sysinfo.rsdp->RsdtAddress);

        madt_discover();
    }

    __asm__("sti");
    serial_println("enabled interupts"); 

    
     
    serial_println("\n==MEM INIT OK==\n");

    initrd(sysinfo.initrd.start, sysinfo.initrd.end - sysinfo.initrd.start);
    
    

  
    
    stdout_init();
    
    gfx_init(color_cyan);
    //we are so back
    
    tasking_init(&task_drawtimer); //first task should be on our main stack as its new kernel main 
    add_task("task_draw_test",task_draw_test);
    //we have gotten ourselves a system with two processes running
    // do a little dance or something 

    printf("highest bit of 0xffff = %i of 8 = %i ", get_highest_bit(0xffff), get_highest_bit(8) );

    ASSERT(gfx_has_init());
    println("randos up");

    //jump_to_usermode(&user_mode_test); //holy shit it worked
 
    
    
   // task_draw_test();
   // start_first_task();
    
    size_t old_len = 0;
    char last_top = stdout_top();
   //  __asm__ volatile("mov rdi, 0xcafebabe;  int 0x0");
    printf("ticks %lu", old_len, last_top, tick);
   
    uint64_t last_tick = 0;
    for(;;){
      //  __asm__("hlt");
        //solve YOUR OLDLEN struggles with one simple TRICK
            //let stdout mark itself as dirty 
        while (old_len != stdout.index ){ //|| last_top != stdout_top() //for backspace 
           
           // if(!stdout_update()) continue;
            gfx_clear_text();
         //   gfx_clear(gfx_state.clear_color);
            gfx_print(get_stdout());
            //for(int i = 0; i < stdout.index; ++i)
           //     gfx_putc(stdout.buffer[stdout.index]);
           
         //   gfx_putc(stdout_top());
           
            old_len = stdout.index;
            last_top = stdout_top();
            break;
             
          //  if(last_top) stdout_putchar('$');
        }
      ///  if((last_tick + 100) < tick  ){
             //   serial_printi("t=", tick);
         //       gfx_clear_line(598, 24);
        //        last_tick = tick;
       //         gfx_print_pos(lltoa(tick, 10), v2(5, 600));
         //   }
         
        
    }

    /*
    
    Scheduler:
        test: one process that reads a bunch of values and updates stdout

        task 1-5: changing values etc

        PIT runs for x interval, then switches task
    */
    for(;;){
        __asm__("hlt");
    }
    


    // kernel ends, we can return to entry pt which hangs or just do it here for transparency while we develop
    __asm__("cli");
    for (;;)
    {
        __asm__("hlt");
    }
    __builtin_unreachable();
}

