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

extern unsigned long GDT_CODE_OFFSET;
void main(void *addr, void *magic)
{
    disable_interupts();
    PIC_init();
   
    init_idt();
    // disable_interupts();
    
    // register irq handlers
    keyboard_init();
    timer_init(50);

 


console_print("WE ARE SO FUCKING BACK");
    console_print("\nIN 64 FUCKIN BITS BABY");

  
 
    console_print(itoa(GDT_CODE_OFFSET, 10));

    if (magic == MULTIBOOT2_BOOTLOADER_MAGIC)
    {
        //  console_print("magic num ok!");
      // serial_print(" mb header ok\n");
    }
    /*
    if(0){

    
        serial_print(" testing out fb stuff");
        struct multiboot_tag *tag;
        void *fb = 0;
        struct multiboot_tag_framebuffer *tagfb;

        struct multiboot_tag_vbe * tagvbe;

        for (tag = (struct multiboot_tag *)(addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
            tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
        {
            if (tag->type = MULTIBOOT_TAG_TYPE_FRAMEBUFFER)
            {
                tagfb = (struct multiboot_tag_framebuffer *)tag;
                fb = (void *)(unsigned long)tagfb->common.framebuffer_addr;
                serial_print("\n found framebuffer tag");
            }   
            if(tag->type = MULTIBOOT_TAG_TYPE_VBE){
                tagvbe = ( struct multiboot_tag_vbe *)tag;
                serial_print("found VBE\n");
            }
        }
        if (fb != 0) //dump fb 
        {
            serial_print("framebuffer is not 0\n");
            serial_print(htoa((uintptr_t)fb));
            serial_print("frame buffer data:\n");
            serial_print("height \n");
            serial_print(itoa(tagfb->common.framebuffer_height, 10));
            serial_print("width \n");
            serial_print(itoa(tagfb->common.framebuffer_width, 10));
            serial_print("bpp \n");
            serial_print(itoa(tagfb->common.framebuffer_bpp, 10));
            serial_print("type \n");
            serial_print(itoa(tagfb->common.framebuffer_type, 10));

            serial_print("\n end frame buf dump");
        }

        if(tagvbe){
            vbe_mode_info_t* vbe = (vbe_mode_info_t*)tagvbe->vbe_mode_info.external_specification;
            serial_print("VBE Info\n");
            serial_print(htoa((uintptr_t)vbe->framebuffer));
            serial_print("\nframe buffer data:\n");
            serial_print("height \n");
            serial_print(itoa(vbe->height, 10));
            serial_print("width \n");
            serial_print(itoa(vbe->width, 10));
            serial_print("bpp \n");
            serial_print(itoa(vbe->bpp, 10));
            serial_print("pitch \n");
            serial_print(itoa(vbe->pitch, 10));

            serial_print("\n end VBE \n");

            serial_print("other shit \n");
            serial_print(itoa(tagvbe->size, 10));
        }
        gfx_init(clr_blue);
        }
    */
    
  console_print("\n 1 init ok");
   __asm__("sti");
   clear_screen();
    console_print("init ok");
    __asm__("int 7");

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