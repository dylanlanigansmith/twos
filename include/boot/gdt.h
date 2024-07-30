#pragma once
#ifdef __INTELLISENSE__
    #include <stdint-gcc.h>
#else
    #include <stdint.h> //this is stupid
#endif


#define GDT_NULL_ENTRY 0x0 //first GDT entry (duh) (for clarity) 

#define KERNEL_CS_OFFSET 0x8 //within gdt, same as we have no access / ring flags
#define KERNEL_DS_OFFSET 0x10 

#define KERNEL_CS 0x8
#define KERNEL_DS 0x10

#define USER_CS_OFFSET 0x18
#define USER_DS_OFFSET 0x20
#define USER_SEL_FLAGS 0x3

#define USER_CS  0x1B // (USER_CS_OFFSET| USER_SEL_FLAGS)
#define USER_DS 0x23 // (USER_DS_OFFSET | USER_SEL_FLAGS)


#define TSS_GDT_OFFSET 0x28

#define GDT_ENTRIES 5  //NOT INCLUDING SYSTEM SEGMENT ENTRIES JUST NUM CS/DS UNTIL SYS
//null, kcs, kds, ucs, uds,

// tss



#define GDT_INIT_QUIET 0
#define GDT_INIT_VERBOSE 1
#define GDT_INIT_ALL 2

void init_gdt_tss(const uint8_t dump);



