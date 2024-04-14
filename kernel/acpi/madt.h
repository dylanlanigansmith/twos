#pragma once

#include "acpi.h"

void madt_discover();


enum MADT_ENTRY_TYPES{
    MADT_PROC_LOCAL_APIC = 0,
    MADT_IO_APIC,
    MADT_IO_APIC_ISO,
    MADT_IO_APIC_NMI_SRC,
    MADT_LOCAL_APIC_NMI,
    MADT_LOCAL_APIC_ADDR_OVERRIDE,
    MADT_PROC_LOCAL_APIC2 = 9

};

typedef struct{
    uint8_t entry_type;
    uint8_t entry_length;
                    
                
} __attribute__((packed)) MADT_entry_descriptor;

typedef struct{
    ACPISDTHeader header;

    //
    uint32_t apic_address;
    uint32_t pic_flags; //1 = dual 8259 PICS
                        //if bit 0 set then 8259s must be masked 
                    
    MADT_entry_descriptor entry; //should be + 0x2C
                
} __attribute__((packed)) MADT_t;


typedef struct{
    MADT_entry_descriptor desc;
    uint8_t apic_proc_id;
    uint8_t apic_id;
    uint32_t flags;
                
} __attribute__((packed)) madt_proc_local_apic; //0


typedef struct{
    MADT_entry_descriptor desc;
    uint8_t bus_src;
    uint8_t irq_src;
    uint32_t gsi; //global system interupt
    uint16_t flags;

                
} __attribute__((packed)) madt_io_apic_src_override; //2

typedef struct{
    MADT_entry_descriptor desc;
    uint16_t res;
    uintptr_t phys_addr;
                
} __attribute__((packed)) madt_apic_x64_override; //5
typedef struct{
    MADT_entry_descriptor desc;
    uint8_t ioapic_id;
    uint8_t zero;
    uint32_t addr;
    uint64_t gsi_base; //global system intrupt
                        //commenting this because it sounds important as i figure this out on the fly
} __attribute__((packed)) madt_io_apic;
//https://wiki.osdev.org/MADT //most of this was actually right!!



//https://wiki.osdev.org/images/9/95/Edge_vs_level.png
#define IOAPIC_FLAG_ACTIVE_LOW (0b0010)
#define IOAPIC_FLAG_LEVEL_TRIGGERED (0b1000)
