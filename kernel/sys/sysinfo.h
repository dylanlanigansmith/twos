#pragma once

#include "../kernel.h"
#include "../acpi/rdsp.h"
#include "../acpi/acpi.h"

#define ACPI_ENTRIES_MAX 8

enum HOST_TYPES {
    UNSURE = 0u, //if we dont know we dont care
    PHYSICAL = 0u,
    QEMU, 
    BOCHS,
    OTHER_VM,
};

extern uint64_t _end;

typedef struct {

    struct{
        char bl_name[16];
        char cmdline[64];
        uintptr_t kernel_base;
        uintptr_t kernel_end;
        uint8_t is_uefi;
    }boot;

    
   


    void* multiboot2;
    void* vbe_mode_info;
    void* framebuffer_common;
    struct {
        uintptr_t addr; 
        uint32_t w, h, pitch, bpp;
    }fb;
    struct{
        void* mb_map;
        size_t mb_size;
    }mem;
  
   

    RSDP_t* rsdp;
    uintptr_t rsdt;

    uint8_t fadt_idx, madt_idx;
    uintptr_t rsdt_entries[ACPI_ENTRIES_MAX] ; //this can be bad like this for now because idek if im brave enough to do ACPI 
                                                //but its important that we COULD/CAN 

    struct {
        uintptr_t start;
        uintptr_t end;
    } initrd;

    uint8_t host_type;
                      
} sysinfo_t;

extern sysinfo_t sysinfo;

static inline void sysinfo_create(sysinfo_t* sys){
    memset(sys, 0, sizeof(sysinfo_t));
   
    sysinfo.boot.kernel_end = &_end; 
    if( sysinfo.boot.kernel_end > KERNEL_ADDR){
        sysinfo.boot.kernel_end -= KERNEL_ADDR;
    }
}