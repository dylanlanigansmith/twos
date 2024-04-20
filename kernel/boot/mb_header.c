#include "mb_header.h"

#include "../../drivers/serial/serial.h"
#include "../../drivers/video/vbe.h"
#include "../sys/sysinfo.h"
#include "cmdline.h"
sysinfo_t sysinfo;

const char* mmap_types[6] = {"mem_unknown", "mem_avail", "mem_reserved", "mem_acpi_reclaimable", "mem_nvs", "mem_badram" };

void handle_tag_vbe(struct multiboot_tag *tag)
{

    struct multiboot_tag_vbe *tagvbe = (struct multiboot_tag_vbe *)tag;

    serial_print("found VBE\n"); // we didnt end up using this (for now)
    vbe_mode_info_t *vbe = (vbe_mode_info_t *)tagvbe->vbe_mode_info.external_specification;
    sysinfo.vbe_mode_info = vbe;
    if (!DUMP_VBE_TAG)
        return;
    serial_printi("vbetag->vbe_mode", tagvbe->vbe_mode);
    serial_printi("vbetag->vbe_interface_off", tagvbe->vbe_interface_off);

    if (tagvbe && vbe)
    {

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

void handle_tag_framebuffer(struct multiboot_tag *tag)
{
    struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
    // if(tagfb->common.framebuffer_width != 1024) continue; //all sorts of garbage comes from this so we should look into it

    sysinfo.framebuffer_common = &tagfb->common;
    sysinfo.fb.addr = tagfb->common.framebuffer_addr;
    sysinfo.fb.w = tagfb->common.framebuffer_width;
    sysinfo.fb.h = tagfb->common.framebuffer_height;
    sysinfo.fb.pitch = tagfb->common.framebuffer_pitch;
    sysinfo.fb.bpp = tagfb->common.framebuffer_bpp;
    debugf("fb @ %lx h= %i p= %i \n", sysinfo.fb.addr, sysinfo.fb.h, sysinfo.fb.pitch);
    if (!DUMP_FB_TAG)
        return;
    serial_print("\n found framebuffer tag");

    serial_print("START Frame Buffer Dump\n");
    serial_printi("tagfb->common.type", tagfb->common.type);
    serial_printi("tagfb->common.size", tagfb->common.size);
    serial_printh("tagfb->common.framebuffer_addr", tagfb->common.framebuffer_addr);
    serial_printi("tagfb->common.framebuffer_pitch", tagfb->common.framebuffer_pitch);
    serial_printi("tagfb->common.framebuffer_width", tagfb->common.framebuffer_width);
    serial_printi("tagfb->common.framebuffer_height", tagfb->common.framebuffer_height);
    serial_printi("tagfb->common.framebuffer_bpp", tagfb->common.framebuffer_bpp);
    serial_printi("tagfb->common.framebuffer_type", tagfb->common.framebuffer_type);
    serial_printi("tagfb->common.reserved", tagfb->common.reserved);
    serial_print("END Frame Buffer Dump\n");
}

void handle_tag_mem_basic(struct multiboot_tag *tag)
{
    if (!DUMP_BASIC_MEM_TAG)
        return;
    serial_println("===MULTIBOOT_TAG_TYPE_BASIC_MEMINFO===");
    struct multiboot_tag_basic_meminfo *meminfo = (struct multiboot_tag_basic_meminfo *)tag;

    serial_printh("meminfo->mem_lower", meminfo->mem_lower);
    serial_printh("meminfo->mem_upper", meminfo->mem_upper);
}


void handle_tag_acpi(struct multiboot_tag *tag)
{
    if (tag->type == MULTIBOOT_TAG_TYPE_ACPI_NEW)
    {
        serial_println("===MMULTIBOOT_TAG_TYPE_ACPI_NEW===");
        // and we hope xtsd backwards compat works for now
    }
    else{
         serial_println("===MULTIBOOT_TAG_TYPE_ACPI_OLD===");
    }
    struct multiboot_tag_old_acpi *acpi = (struct multiboot_tag_old_acpi *)tag;

    RSDP_t *rsdp = (RSDP_t *)acpi->rsdp;

    if (!validate_rsdp(rsdp))
    {
        serial_println("FAILED TO VALIDATE RSDP! ");
        sysinfo.rsdp = 0;
        return;
    }
    sysinfo.rsdp = rsdp;

    if (!DUMP_ACPI_TAG)
        return;

    char sig[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    memcpy(sig, rsdp->Signature, 8); // so we can print it lol its always gonna b 'RSD TPTR ' but not nullterm

   
    debugf("rsdp->Signature: '%s' \n", sig);         // not null terminated btw
    debugf("rsdp->OEMID: '%s' \n", rsdp->OEMID);     // not null terminated btw
    serial_printi("rsdp->Revision", rsdp->Revision); // so acpi2 is backwards compat maybe we can just impl this
    serial_printi("rsdp->Checksum", rsdp->Checksum);
    serial_printh("rsdp->RsdtAddress", rsdp->RsdtAddress); // 32 bit PHYSICAL
    // so me think acpi is mmap [4]:{ @0xbffe0000 - 0xc0000000 } size 0x20000 |  mem_reserved 2

    // gotta page that to check it out
    //   ACPI_discover_SDTs( (void*)((uintptr_t)(rsdp->RsdtAddress)));
}


void handle_tag_mmap(struct multiboot_tag *tag)
{
    struct multiboot_tag_mmap *mmaps = (struct multiboot_tag_mmap *)tag;

    sysinfo.mem.mb_map = mmaps;
    sysinfo.mem.mb_size = (mmaps->size - sizeof(struct multiboot_tag_mmap)) / (size_t)mmaps->entry_size;
    if (!DUMP_MMAP_TAG)
        return;
    debugf("===MULTIBOOT_TAG_TYPE_MMAP===");
    debugf("mmaps->total_size=%i mmaps->entry_size=%i mmaps->entry_version=%i\n num_entries = %li",mmaps->size, mmaps->entry_size,  mmaps->entry_version,  sysinfo.mem.mb_size);
    //(size_t)( (sizeof(struct multiboot_tag_mmap) - sizeof(multiboot_memory_map_t)) + (mmaps->e)  
 
   
    for (int i = 0; i <  sysinfo.mem.mb_size; ++i)
    {

        multiboot_memory_map_t mmap = mmaps->entries[i];
        uint32_t type = (mmap.type > 5) ? 0 : mmap.type;
        debugf("mmap [%i]:{ @%lx - %lx } size %lx |  %s %i \n", i,
               mmap.addr, mmap.addr + mmap.len, mmap.len, mmap_types[type], mmap.type);
    }
}


void handle_tag_module(struct multiboot_tag *tag)
{
    static int mods_found = 0;

    struct multiboot_tag_module *mod = (struct multiboot_tag_module *)tag;
    

    sysinfo.initrd.start = mod->mod_start ;
    sysinfo.initrd.end = mod->mod_end ;
    //could check CRC here to ensure its our initrd
    //but then we have edge cases where its not ID mapped
        //if that can even happen?
    //fine for now let initrd handle that 
    //will be an issue if theres more than one module
    mods_found++;
    if(mods_found > 1){
        debugf("WARNING WARNING WARNING WARNING MORE THAN ONE [%i] MB2 MODULE! WE DO NOT HANDLE THIS YET!\n", mods_found);
    }
    if(!DUMP_MODULE_TAG && mods_found == 1){
        debugf("Found Multiboot Module at %lx. Total: %i\n", mod->mod_start, mods_found); return;
    }
    debugf("MULTIBOOT_TAG_TYPE_MODULE [%i]\n", mods_found);
    debugf("mod start = %x end = %x size = %u \n", mod->mod_start, mod->mod_end, mod->mod_end - mod->mod_start);
}



void handle_tag_string(struct multiboot_tag *tag, const char* label, char* buf, size_t size)
{
    struct multiboot_tag_string* stag = (struct multiboot_tag_string*)tag;
    //https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html#Boot-loader-name
    //null terminated normal strings!? Thanks Multiboot!
    

    size_t stag_len = stag->size - sizeof(struct multiboot_tag);
    if(label != 0){
        const char* to_print = stag->string;
         if (!stag_len)
            to_print = "[empty]";
        debugf("MB2 %s = '%s' \n", label, to_print);
    }
    

    if(buf != 0 && size > 0 && stag_len){
        strncpy(buf, stag->string, size);
    }
}

int get_multiboot_initial_info(void *header, uint64_t magic)
{
    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) return MB_HEADER_PARSE_ERROR;
   // debugf("bootloader ok\n");
    sysinfo_create(&sysinfo); // memset sysinfo, init defaults

    struct multiboot_tag *tag = (struct multiboot_tag *)((uintptr_t)header + 8);
    
    while(tag->type != MULTIBOOT_TAG_TYPE_END){
        if(tag->type == MULTIBOOT_TAG_TYPE_CMDLINE){
            int num_args = cmdline_parse( (struct multiboot_tag_string*)tag );
           // debugf("found %i cmdline args.\n", num_args);
            return MB_HEADER_PARSE_SUCCESS;
        }


        tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
    }

   // debugf("no mb2 cmdline tag present.\n");
    return MB_HEADER_PARSE_SUCCESS;
}

int parse_multiboot_header(void *header, uint64_t magic)
{
    if(magic != MULTIBOOT2_BOOTLOADER_MAGIC) return MB_HEADER_PARSE_ERROR;

   
    struct multiboot_tag *tag;
  

    sysinfo.multiboot2 = header;
    

    bool are_we_uefi = False;

    serial_println("==Parsing Multiboot2 Header==");

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
        switch (tag->type)
        {
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
                handle_tag_string(tag, 0,sysinfo.boot.bl_name,16); break;
            case MULTIBOOT_TAG_TYPE_CMDLINE:
                handle_tag_string(tag, 0,sysinfo.boot.cmdline,64); break;
            case MULTIBOOT_TAG_TYPE_LOAD_BASE_ADDR:
                sysinfo.boot.kernel_base = (uintptr_t)((struct multiboot_tag_load_base_addr*)(tag))->load_base_addr;   break;
    
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
                handle_tag_framebuffer(tag); break;
            case MULTIBOOT_TAG_TYPE_VBE:
                handle_tag_vbe(tag); break;
            case  MULTIBOOT_TAG_TYPE_ACPI_OLD:
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
                handle_tag_acpi(tag); break;
            case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
                handle_tag_mem_basic(tag); break;
            case MULTIBOOT_TAG_TYPE_MODULE:
                handle_tag_module(tag); break;
            case MULTIBOOT_TAG_TYPE_MMAP:
                handle_tag_mmap(tag); break;

            case MULTIBOOT_TAG_TYPE_EFI64:
            case MULTIBOOT_TAG_TYPE_EFI_MMAP:
            case MULTIBOOT_TAG_TYPE_EFI_BS:
                are_we_uefi = True; 

            default:
                if(DUMP_UNKNOWN_TAGS){
                    debugf("Unhandled Multiboot Tag: %u\n", tag->type);
                }
        };
    }

    sysinfo.boot.is_uefi = are_we_uefi;
    serial_println("==parsed mb2 header==");
    debugf("boot info: %s [%s] cmdline='%s' kernel_base=%lx\n",(sysinfo.boot.is_uefi) ? "UEFI" : "BIOS",  sysinfo.boot.bl_name, sysinfo.boot.cmdline, sysinfo.boot.kernel_base);
    return MB_HEADER_PARSE_SUCCESS;
}