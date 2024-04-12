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

#include "../drivers/serial/serial.h"

#include "mem/page.h"
#include "mem/heap.h"

#include "task/task.h"
#include "stdio/stdout.h"
extern unsigned long GDT_CODE_OFFSET;

int cpp_test(int, int);


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
             //   serial_printi("t=", tick);
                gfx_clear_line(498, 24);
                last_tick2 = tick;
                gfx_print_pos(lltoa(tick, 10), v2(5, 500));
            }
    }
}

void main(void *addr, void *magic)
{
    disable_interupts();
    PIC_init();
   
    init_idt();
    // disable_interupts();
    
    // register irq handlers
    keyboard_init();
    timer_init(PIT_RATE);

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC){
        //uh what?
    }
    
    

    serial_init();
    serial_printh("multiboot addr",  addr);
    
    struct multiboot_tag *tag;
    struct multiboot_tag_framebuffer *tagfb;

    struct multiboot_tag_vbe * tagvbe;

    for (tag = (struct multiboot_tag *)(addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        if (tag->type = MULTIBOOT_TAG_TYPE_FRAMEBUFFER)
        {
            tagfb = (struct multiboot_tag_framebuffer *)tag;
            if(tagfb->common.framebuffer_width != 1024) continue;
            serial_print("\n found framebuffer tag");
           
            serial_print("START Frame Buffer Dump\n");
            serial_printi("tagfb->common.type",  tagfb->common.type);
            serial_printi("tagfb->common.size",  tagfb->common.size);
            serial_printh("tagfb->common.framebuffer_addr",  tagfb->common.framebuffer_addr);
            serial_printi("tagfb->common.framebuffer_pitch",  tagfb->common.framebuffer_pitch);
            serial_printi("tagfb->common.framebuffer_width",  tagfb->common.framebuffer_width);
            serial_printi("tagfb->common.framebuffer_height",  tagfb->common.framebuffer_height);
            serial_printi("tagfb->common.framebuffer_bpp",  tagfb->common.framebuffer_bpp);
            serial_printi("tagfb->common.framebuffer_type",  tagfb->common.framebuffer_type);
            serial_printi("tagfb->common.reserved",  tagfb->common.reserved);
            serial_print("END Frame Buffer Dump\n");          
        }   
        if(tag->type = MULTIBOOT_TAG_TYPE_VBE){
            tagvbe = ( struct multiboot_tag_vbe *)tag;
            serial_print("found VBE\n");
        }
    }
  
    _init_cpp(); //bc we cant get global constructors to work & by we i mean I
   __asm__("sti");
    serial_println("enabled interupts");
   
    make_page_struct(); //this also initializes heap, maps frame buffer
    
    stdout_init();
    gfx_init(color_cyan);
    //we are so back
    
    tasking_init(&task_drawtimer); //first task should be on our main stack as its new kernel main 
    add_task("task_draw_test",task_draw_test);
    //we have gotten ourselves a system with two processes running
    // do a little dance or something 



    ASSERT(gfx_has_init());
    
     printf("tasking time :( %lx %lx \n", (uintptr_t)task_drawtimer, (uintptr_t)task_draw_test);
    
   // task_draw_test();
    start_first_task();

    size_t old_len = 0;
    char last_top = stdout_top();
    printf("old len = %li last top %c", old_len, last_top);

    uint64_t last_tick = 0;
    for(;;){
        __asm__("hlt");
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
    /*
        now what?

        kernel heap //in progress//
        at least get a malloc

            then we gotta figure out page api
            then we can do fork/tasks
            then jump to user mode 

            goal: game in USER MODE 
                also lets write game in cpp please

                 c++ ?
                 cross compiler ?
        user mode ?


        FILES = RAM DISK

    
    */


    // kernel ends, we can return to entry pt which hangs or just do it here for transparency while we develop
    __asm__("cli");
    for (;;)
    {
        __asm__("hlt");
    }
    __builtin_unreachable();
}

