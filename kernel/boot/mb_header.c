#include "mb_header.h"

#include "../../drivers/serial/serial.h"
#include "../../drivers/video/vbe.h"
#include "../sys/sysinfo.h"

sysinfo_t sysinfo;

const char* mmap_types[6] = {"mem_unknown", "mem_avail", "mem_reserved", "mem_acpi_reclaimable", "mem_nvs", "mem_badram" };

void parse_tag_vbe(struct multiboot_tag_vbe * tagvbe)
{
     serial_print("found VBE\n"); //we didnt end up using this (for now)
     vbe_mode_info_t* vbe = (vbe_mode_info_t*)tagvbe->vbe_mode_info.external_specification;
     sysinfo.vbe_mode_info = vbe;
    if(!DUMP_VBE_TAG) return;
    serial_printi("vbetag->vbe_mode", tagvbe->vbe_mode);
    serial_printi("vbetag->vbe_interface_off", tagvbe->vbe_interface_off);

    if(tagvbe && vbe ){
       
        serial_printi("vbe_mode_info->attributes", vbe->attributes);
        serial_printi("vbe_mode_info->window_a", vbe->window_a);
        serial_printi("vbe_mode_info->window_b", vbe->window_b);
        serial_printi("vbe_mode_info->granularity", vbe->granularity);
        serial_printi("vbe_mode_info->window_size", vbe->window_size);
        serial_printi("vbe_mode_info->segment_a", vbe->segment_a);
        serial_printi("vbe_mode_info->segment_b", vbe->segment_b);
        serial_printh("vbe_mode_info->win_func_ptr", vbe->win_func_ptr);
        serial_printi("vbe_mode_info->pitch", vbe->pitch);
        serial_printi("vbe_mode_info->width", vbe->width);
        serial_printi("vbe_mode_info->height", vbe->height);
        serial_printi("vbe_mode_info->w_char", vbe->w_char);
        serial_printi("vbe_mode_info->y_char", vbe->y_char);
        serial_printi("vbe_mode_info->planes", vbe->planes);
        serial_printi("vbe_mode_info->bpp", vbe->bpp);
        serial_printi("vbe_mode_info->banks", vbe->banks);
        serial_printi("vbe_mode_info->memory_model", vbe->memory_model);
        serial_printi("vbe_mode_info->bank_size", vbe->bank_size);
        serial_printi("vbe_mode_info->image_pages", vbe->image_pages);
        serial_printi("vbe_mode_info->reserved0", vbe->reserved0);
        serial_printi("vbe_mode_info->red_mask", vbe->red_mask);
        serial_printi("vbe_mode_info->red_position", vbe->red_position);
        serial_printi("vbe_mode_info->green_mask", vbe->green_mask);
        serial_printi("vbe_mode_info->green_position", vbe->green_position);
        serial_printi("vbe_mode_info->blue_mask", vbe->blue_mask);
        serial_printi("vbe_mode_info->blue_position", vbe->blue_position);
        serial_printi("vbe_mode_info->reserved_mask", vbe->reserved_mask);
        serial_printi("vbe_mode_info->reserved_position", vbe->reserved_position);
        serial_printi("vbe_mode_info->direct_color_attributes", vbe->direct_color_attributes);
        serial_printh("vbe_mode_info->framebuffer", vbe->framebuffer);
        serial_printi("vbe_mode_info->off_screen_mem_off", vbe->off_screen_mem_off);
        serial_printi("vbe_mode_info->off_screen_mem_size", vbe->off_screen_mem_size);
    }

}


int parse_multiboot_header(void *header, uint64_t magic)
{
    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) return MB_HEADER_PARSE_ERROR;

    sysinfo_create(&sysinfo);
    struct multiboot_tag *tag;
    struct multiboot_tag_framebuffer *tagfb;

    sysinfo.multiboot2 = header;

    serial_println("parse_multiboot_header()");

    for (tag = (struct multiboot_tag *)((uintptr_t)header + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        /*
            tags we found on qemu:
            MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR
            MULTIBOOT_TAG_TYPE_CMDLINE 
            MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME
            MULTIBOOT_TAG_TYPE_BASIC_MEMINFO
            MULTIBOOT_TAG_TYPE_BOOTDEV
            MULTIBOOT_TAG_TYPE_MMAP
            MULTIBOOT_TAG_TYPE_APM
            MULTIBOOT_TAG_TYPE_ACPI_OLD
        */


        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER)
        {
            tagfb = (struct multiboot_tag_framebuffer *)tag;
           // if(tagfb->common.framebuffer_width != 1024) continue; //all sorts of garbage comes from this so we should look into it 

           sysinfo.framebuffer_common = &tagfb->common;
           if(!DUMP_FB_TAG) continue;
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
            continue;    
        }   
        if(tag->type == MULTIBOOT_TAG_TYPE_VBE){
            struct multiboot_tag_vbe * tagvbe = ( struct multiboot_tag_vbe *)tag;
            if(!tagvbe) continue;
            
           
            parse_tag_vbe(tagvbe);
        }

        if(tag->type == MULTIBOOT_TAG_TYPE_ACPI_OLD){
            

            struct multiboot_tag_old_acpi* acpi = (struct multiboot_tag_old_acpi*)tag;

            RSDP_t* rsdp = (RSDP_t*) acpi->rsdp;

            sysinfo.rsdp = rsdp;
            
            if(!DUMP_ACPI_TAG) continue;

            char sig[9] = {0,0,0,0,0,0,0,0,0};
            memcpy(sig, rsdp->Signature, 8); //so we can print it lol its always gonna b 'RSD TPTR ' but not nullterm

            serial_println("===MULTIBOOT_TAG_TYPE_ACPI_OLD===");
            debugf("rsdp->Signature: '%s' \n", sig); //not null terminated btw
             debugf("rsdp->OEMID: '%s' \n", rsdp->OEMID); //not null terminated btw
            serial_printi("rsdp->Revision", rsdp->Revision); //so acpi2 is backwards compat maybe we can just impl this
            serial_printi("rsdp->Checksum", rsdp->Checksum);
            serial_printh("rsdp->RsdtAddress", rsdp->RsdtAddress); //32 bit PHYSICAL
            //so me think acpi is mmap [4]:{ @0xbffe0000 - 0xc0000000 } size 0x20000 |  mem_reserved 2 

            //gotta page that to check it out
         //   ACPI_discover_SDTs( (void*)((uintptr_t)(rsdp->RsdtAddress)));
             continue;
        }
        if(tag->type == MULTIBOOT_TAG_TYPE_BASIC_MEMINFO)
        {
            if(!DUMP_BASIC_MEM_TAG) continue;
            serial_println("===MULTIBOOT_TAG_TYPE_BASIC_MEMINFO===");
            struct multiboot_tag_basic_meminfo* meminfo = (struct multiboot_tag_basic_meminfo*)tag;

            serial_printh("meminfo->mem_lower", meminfo->mem_lower);
            serial_printh("meminfo->mem_upper", meminfo->mem_upper);
        }
        if(tag->type == MULTIBOOT_TAG_TYPE_MMAP)
        {
           
            struct multiboot_tag_mmap* mmaps = (struct multiboot_tag_mmap*)tag;
            
            sysinfo.mb_mmaps = mmaps;

            if(!DUMP_MMAP_TAG) continue;
             serial_println("===MULTIBOOT_TAG_TYPE_MMAP===");
            serial_printi("mmaps->entry_size", mmaps->entry_size);
            serial_printh("mmaps->entry_version", mmaps->entry_version);
            for(int i = 0; i < mmaps->entry_size; ++i){

                multiboot_memory_map_t mmap = mmaps->entries[i];
                uint32_t type = (mmap.type > 5) ? 0 : mmap.type; 
                debugf("mmap [%i]:{ @%lx - %lx } size %lx |  %s %i \n",i, 
                        mmap.addr, mmap.addr + mmap.len, mmap.len, mmap_types[type], mmap.type);
            }
        }
         if(tag->type == MULTIBOOT_TAG_TYPE_ACPI_NEW){
                serial_println("===MMULTIBOOT_TAG_TYPE_ACPI_NEW===");
         }
    }
    serial_println("parsed mb2 header successfully");
    return MB_HEADER_PARSE_SUCCESS;
}