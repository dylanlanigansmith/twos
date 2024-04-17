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



typedef struct {
    uint8_t is_uefi;


    void* multiboot2;
    void* vbe_mode_info;
    void* framebuffer_common;
    struct {
        uintptr_t addr; 
        uint32_t w, h, pitch, bpp;
    }fb;
    void* mb_mmaps;

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

    memset(sysinfo.rsdt_entries,0, ACPI_ENTRIES_MAX * sizeof(uintptr_t) );
}