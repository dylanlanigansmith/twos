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
void main_fn()
{
bed:
   // if(first_call) panic("omg");
    first_call = True;
    println("task main");
    start_tick = 0;
    //gfx_clear(color_cyan);
    while( 0xffffffffff < (tick) ){
        //__asm__ volatile ("hlt");
    }
    printi(tick);
    println("---");println("---");println("---");println("---");println("---");
    tick = 0;
    yield();

    println("main: im back bitches");

    //goto bed;

    for(;;){
        __asm__("hlt");
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
    timer_init(5);

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
   // print("we are so fucking back baby \n");
   // print("IN 64 BIT WITH GRUB SUPPORT\n");
   // print("AND 32 BIT COLOR\n");

    //tasking_init(main_fn);
    //main_fn();
    //we have gotten ourselves a system

  
   


    
    
    //print(">");

    //char* noway = _malloc(24);


   // uint32_t i = *(uint32_t*)(HEAP_VIRT_REAL); //there is many a bug in your paging code

   // strcpy(noway, "I JUST MALLOCED");
   // printf("our heap is at %lx and btw %s from it\n", (uint64_t)(HEAP_VIRT_REAL), noway);
    //
    //_free(noway);

   // printf("\n i just freed our string %s \n", noway);
    //noway = kmalloc(24);
  //  strcpy(noway, "HOLY SHIT IT WORKED");

   // printf("now I malloc a new string, did it work? %s \n", noway);

   // println("now i am going to malloc 10mb, sorry");

   // void* hate2seeit = kmalloc(PAGE_SIZE * 5);

   // kfree(hate2seeit);
   // println("well, i just freed that 10mb, are you telling me this shit worked?");
    //println("freed");
   // int agh = cpp_test(2,2);
   
   //println("task this bitches");
   // gfx_print(stdout.buffer);

  // gfx_clear(color_cyan);

 // print_stdout("what the fuck");
    

    ASSERT(gfx_has_init());
    
     print("tasking time :(");
    
    size_t old_len = 0;
    char last_top = stdout_top();
    printf("old len = %li last top %c", old_len, last_top);
    for(;;){
        __asm__("hlt");
        while (old_len != stdout.index || last_top != stdout_top()){ //for backspace 
           
           // if(!stdout_update()) continue;
            gfx_clear_text();
         //   gfx_clear(gfx_state.clear_color);
            gfx_print(get_stdout());
            //for(int i = 0; i < stdout.index; ++i)
           //     gfx_putc(stdout.buffer[stdout.index]);
           
         //   gfx_putc(stdout_top());
           
            old_len = stdout.index;
            last_top = stdout_top();

          //  if(last_top) stdout_putchar('$');
        }
    }

    
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

