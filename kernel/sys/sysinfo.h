#pragma once

#include "../kernel.h"
#include "../acpi/rdsp.h"
#include "../acpi/acpi.h"
typedef struct {
    void* multiboot2;
    void* vbe_mode_info;
    void* framebuffer_common;

    void* mb_mmaps;

    RSDP_t* rsdp;
} sysinfo_t;

extern sysinfo_t sysinfo;

static inline void sysinfo_create(sysinfo_t* sys){
    memset(sys, 0, sizeof(sysinfo_t));
}