#pragma once
#include <kstdlib.h>

#include "acpi_structs.h"


typedef struct {
    rsdp_t* rsdp;
    rsdt_t* rsdt;
    madt_t* madt;
    
    struct{
      uint64_t    proc_apic;
      uint64_t      io_apic;
      uint64_t    ioso_apic;
      uint64_t   ionmi_apic;
      uint64_t    nmi_lapic;
      uint64_t  addro_lapic;
      uint64_t procx2_lapic;
    }idx;
    struct{
      proc_lapic_t**   proc_apic;
      io_apic_t**      io_apic;
      ioso_apic_t**    ioso_apic;
      ionmi_apic_t**   ionmi_apic;
      nmi_lapic_t**    nmi_lapic;
      addro_lapic_t**  addro_lapic;
      procx2_lapic_t** procx2_lapic;
    } tbl;
    
    bool is_v2;
    bool debug_madt;
    
} acpi_inst_t ;

extern acpi_inst_t acpi;

void acpi_init(rsdp_t* rsdp);

static inline int acpi_validate_rsdp(rsdp_t* rsdp){
    if(!rsdp) return 0;
    uint8_t sum = 0;
    uint8_t* p = (uint8_t*) rsdp;
    for(uint64_t i = 0; i < RSDP_CHECKSUM_VALIDATE_SIZE; ++i){
        sum += p[i];
    }
    return ((sum & 0xFF) == 0);
}

static bool acpi_validate_sig(acpi_sdt_header_t* header, const char* sig) {
    return (!strncmp(header->signature, sig, SDT_SIG_LEN)); 
}

static bool acpi_validate_checksum(acpi_sdt_header_t* header) {
    uint8_t sum = 0;
    uint8_t* ptr = (uint8_t*)header;

    for (uint32_t i = 0; i < header->length; i++) {
        sum += ptr[i];
    }

    return ((sum & 0xFF) == 0) ;
}

