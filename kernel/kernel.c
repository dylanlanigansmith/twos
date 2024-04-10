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
    
    

    serial_init();
    serial_printh("multiboot addr",  addr);
    
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
    /**
     * we find many a frame buffer
     * the one we asked for is there!
     * it is at 0xfc000000 
     * we GET A PAGE FAULT when we go for it
     * 
     * SO
     * we need paging
     * how?
     * idk
     * but we gotta figure out how to do it from C
     * then we can malloc and oml
     * 32 bit color
    */

    if(tagvbe && 0){
        vbe_mode_info_t* vbe = (vbe_mode_info_t*)tagvbe->vbe_mode_info.external_specification;
        serial_printi("vbe->attributes", vbe->attributes);
        serial_printi("vbe->window_a", vbe->window_a);
        serial_printi("vbe->window_b", vbe->window_b);
        serial_printi("vbe->granularity", vbe->granularity);
        serial_printi("vbe->window_size", vbe->window_size);
        serial_printi("vbe->segment_a", vbe->segment_a);
        serial_printi("vbe->segment_b", vbe->segment_b);
        serial_printi("vbe->win_func_ptr", vbe->win_func_ptr);
        serial_printi("vbe->pitch", vbe->pitch);
        serial_printi("vbe->width", vbe->width);
        serial_printi("vbe->height", vbe->height);
        serial_printi("vbe->w_char", vbe->w_char);
        serial_printi("vbe->y_char", vbe->y_char);
        serial_printi("vbe->planes", vbe->planes);
        serial_printi("vbe->bpp", vbe->bpp);
        serial_printi("vbe->banks", vbe->banks);
        serial_printi("vbe->memory_model", vbe->memory_model);
        serial_printi("vbe->bank_size", vbe->bank_size);
        serial_printi("vbe->image_pages", vbe->image_pages);
        serial_printi("vbe->reserved0", vbe->reserved0);
        serial_printi("vbe->red_mask", vbe->red_mask);
        serial_printi("vbe->red_position", vbe->red_position);
        serial_printi("vbe->green_mask", vbe->green_mask);
        serial_printi("vbe->green_position", vbe->green_position);
        serial_printi("vbe->blue_mask", vbe->blue_mask);
        serial_printi("vbe->blue_position", vbe->blue_position);
        serial_printi("vbe->reserved_mask", vbe->reserved_mask);
        serial_printi("vbe->reserved_position", vbe->reserved_position);
        serial_printi("vbe->direct_color_attributes", vbe->direct_color_attributes);
        serial_printh("vbe->framebuffer", vbe->framebuffer);
        serial_printi("vbe->off_screen_mem_off", vbe->off_screen_mem_off);
        serial_printi("vbe->off_screen_mem_size", vbe->off_screen_mem_size);
// Note: Printing reserved1, which is an array, might not provide useful information in debug prints

    }
    
  
  console_print("\n 1 init ok");
   __asm__("sti");
   clear_screen();
    console_print("init ok");

    make_page_struct();
    serial_println("trying out memory!");
   // serial_printh("map = ", *(uint64_t*)(VIRTMAP)); 
  
   color cyan;

   cyan.r = 0;
   cyan.g = 0xff;
   cyan.b = 0xff;
   cyan.a = 0xff;
  
   


   
     gfx_init(cyan);

     gfx_print("no fucking way");
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