#include <kernel/kernel.h>
#include <boot/multiboot2.h>
#include <boot/cmdline.h>
#include <boot/gdt.h>

#include <kernel/printk.h>
#include <kernel/driver/drivers.h>
#include <kernel/port/port.h>
#include <kernel/mem/physmem.h>
#include <kernel/mem/virtmem.h>
#include <kernel/mem/kalloc.h>

#include <kernel/acpi/acpi.h>
#include <kernel/apic/apic.h>
#include <kernel/apic/ioapic.h>
#include <kernel/isr/idt.h>
#include <kernel/isr/isr.h>


 void test_handler(registers_t* reg){
    debugf("test handler handled that shit!");        
}

#define BOOT_STAGE(str) serial_println(str);
kernel_boot_t kboot;

void kmain(struct multiboot_header* header, uint64_t magic)
{
    if(unlikely( magic != MULTIBOOT2_BOOTLOADER_MAGIC) )
        panic("MB MAGIC MISMATCH");
    serial_init(); //we are in development
    BOOT_STAGE("==twOS==")
    init_gdt_tss(0);
    idt_init();

    int args = 0;
    size_t total_mem = 0;
    rsdp_t *rsdp = nullptr;
    kboot.is_legacy_bios = true;
    memset(&kboot, 0, sizeof(kernel_boot_t));
    struct multiboot_tag *tag = (struct multiboot_tag *)((uintptr_t)header + 8);
    BOOT_STAGE("PARSING BOOTLOADER INFO");
    for (tag = (struct multiboot_tag *)((uintptr_t)header + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
        tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
    {
        switch(tag->type){
            case MULTIBOOT_TAG_TYPE_CMDLINE:{
                if (((struct multiboot_tag_string*)(tag))->string)
                    printk(LOG_INFO, "cmdline=%s", ((struct multiboot_tag_string*)(tag))->string);
                if(cmdline_parse( (struct multiboot_tag_string*)tag )){
                    kboot.dbg.use_serial = (cmdline_getarg("serial") != CMDLINE_NO_ARG);
                    kboot.dbg.level = (cmdline_hasarg("dbg")) ? cmdline_getarg("dbg") : 0;
                } 
                continue;
            }
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:{
                if (((struct multiboot_tag_string*)(tag))->string)
                    printk(LOG_INFO, "bootloader=%s", ((struct multiboot_tag_string*)(tag))->string);
            }
            case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:{
                struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
                if(!tagfb->common.framebuffer_addr || tagfb->common.framebuffer_addr > (0x100000000000) || tagfb->common.framebuffer_width < 64 || tagfb->common.framebuffer_height < 64  ) continue;
                kboot.fb.ptr = 0;
                kboot.fb.addr = tagfb->common.framebuffer_addr;
                kboot.fb.w = tagfb->common.framebuffer_width;
                kboot.fb.h = tagfb->common.framebuffer_height;
                kboot.fb.pitch = tagfb->common.framebuffer_pitch;
                kboot.fb.bpp = tagfb->common.framebuffer_bpp;
                printk(LOG_INFO,"framebuffer %lX {%ux%u}(%u) [%ubpp]",kboot.fb.addr, kboot.fb.w, kboot.fb.h, kboot.fb.pitch, kboot.fb.bpp  ); 
                continue;
            }
            case MULTIBOOT_TAG_TYPE_MMAP:{
                 struct multiboot_tag_mmap *mmaps = (struct multiboot_tag_mmap *)tag;
                kboot.mb.mmap_size = (mmaps->size - sizeof(struct multiboot_tag_mmap)) / (size_t)mmaps->entry_size;
                
                if(cmdline_numargs() && cmdline_hasarg("dumpmem")){
                    const char* mmap_types[6] = {"mem_unknown", "mem_avail", "mem_reserved", "mem_acpi_reclaimable", "mem_nvs", "mem_badram" };
                    debugf("===MULTIBOOT_TAG_TYPE_MMAP===");
                    debugf("mmaps->total_size=%i mmaps->entry_size=%i mmaps->entry_version=%i\n num_entries = %li",mmaps->size, mmaps->entry_size,  mmaps->entry_version,  kboot.mb.mmap_size);
                    for (int i = 0; i <  kboot.mb.mmap_size; ++i){
                        multiboot_memory_map_t mmap = mmaps->entries[i];
                        uint32_t type = (mmap.type > 5) ? 0 : mmap.type;
                        debugf("mmap [%i]:{ @%lx - %lx } size %lx |  %s %i \n", i,
                            mmap.addr, mmap.addr + mmap.len, mmap.len, mmap_types[type], mmap.type);
                    }
                }
                
                //PMM INIT
                total_mem = physmem_init(mmaps, kboot.mb.mmap_size);
                if(!total_mem)  panic("PMM Fail - Init Failed");

                BOOT_STAGE("Physical Memory Manager READY");
                printk(LOG_INFO, "%li MiB of Physical Memory Available", BYTES_TO_MIB(total_mem));
                continue;
            }
            case MULTIBOOT_TAG_TYPE_EFI_BS:
            case MULTIBOOT_TAG_TYPE_EFI_MMAP:
            case MULTIBOOT_TAG_TYPE_EFI32:
            case MULTIBOOT_TAG_TYPE_EFI64:{
                debugf("BOOTING VIA EFI %u", tag->type); 
                kboot.is_legacy_bios = false;
                continue;
            }
            case MULTIBOOT_TAG_TYPE_ACPI_NEW:
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:{
                struct multiboot_tag_old_acpi *acpi_tag = (struct multiboot_tag_old_acpi *)tag;
                if(rsdp != nullptr){
                    if(rsdp->revision > ((rsdp_t *)acpi_tag->rsdp)->revision)
                        continue;
                }

                rsdp = (rsdp_t *)acpi_tag->rsdp;
                if(!acpi_validate_rsdp(rsdp)) panic("ACPI - RSDP Invalid?!");
                debugf("ACPI - RSDP v%i FOUND @ %lx", rsdp->revision, (uintptr_t)rsdp );
                continue;
            }

            default:{
                continue;
            }
        }
    }

    if(!total_mem)  panic("PMM Fail - No MB MMAP?");
    virtmem_init();
    BOOT_STAGE("Virtual Memory Manager READY");

    kalloc_init();
    void* malloc_test = kmalloc(512);
    debugf("malloced all over the place %lx", malloc_test);
    kfree(malloc_test);
    BOOT_STAGE("Kernel Heap READY");

    BOOT_STAGE("Framebuffer READY");
    framebuffer_display_test_pattern(kboot.fb.ptr, kboot.fb.w, kboot.fb.h, kboot.fb.pitch);
    //at this point we now have:
    //  interupts for cpu exception handling
    //  phys mem management
    //  virtual memory management 
    //  kernel heap (kmalloc)
    //  framebuffer info, ID mapped for now
    
    acpi_init(rsdp);
    BOOT_STAGE("ACPI Information READY");

    apic_init();
    BOOT_STAGE("APIC READY");
    ioapic_init();
    BOOT_STAGE("IOAPIC READY");

    __asm__ volatile ("int 3;");

   
    register_interupt_handler(IRQ15, test_handler);

    enable_interupts();

    __asm__ volatile ("int 47;");
    



  /*
        twOS bootup next stages:
        - we need acpi and shit
        - consoles and streams
        - ring buffers
    */



    if(cmdline_hasarg("autooff"))
        port_word_out(0x604, 0x0 | 0x2000);
    __asm__("cli");
    for (;;)
        __asm__("hlt");
    __builtin_unreachable();
}

