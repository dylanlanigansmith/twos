#pragma once
#include <kstdlib.h>
#include <kernel/mem/page.h>

#define KERNEL_VADDR 0xffffffff80000000 //our higher half start point
#define KERNEL_VADDR_SIZE ONE_GIB //the initial ID map 







typedef uintptr_t phys_addr_t;

typedef struct {
    struct{
        uint8_t use_serial;
        uint8_t use_vm;
        uint8_t level; 
    }dbg;

    struct{
        void* first_tag;
        void* mmap;
        size_t mmap_size;
    } mb;
    struct {
        void* ptr;
        phys_addr_t addr;
        size_t w, h, pitch, bpp;
    } fb;
    

}  kernel_boot_t;

extern kernel_boot_t kboot;