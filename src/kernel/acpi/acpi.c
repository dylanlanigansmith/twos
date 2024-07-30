#include <kernel/acpi/acpi.h>
#include <kernel/mem/physmem.h>
#include <kernel/mem/virtmem.h>
#include <kernel/mem/kalloc.h>
#include <kernel/printk.h>
/*
  pretty much all the logic and structs here is stolen from osdev or 
  I will quote linus torvalds from 2003 to explain why:
        "ACPI is a complete design disaster in every way. 
        But we're kind of stuck with it. If any Intel people are listening to this,
        and you had anything to do with ACPI, shoot yourself now, before you reproduce." 
   - http://www.linuxjournal.com/article/7279


*/



acpi_inst_t acpi;


acpi_sdt_header_t* acpi_find_tbl(const char* sig) {
    uint64_t num_entries = (acpi.rsdt->header.length - sizeof(acpi.rsdt->header)) / (acpi.is_v2 ? 8 : 4);
    acpi_sdt_header_t* entry;
    for (uint64_t i = 0; i < num_entries; i++) {
        entry = (acpi_sdt_header_t*)((uintptr_t)acpi.rsdt->entries[i]);
        if (acpi_validate_sig(entry, sig) && acpi_validate_checksum(entry)) {
            return (acpi_sdt_header_t*)((uintptr_t)entry);
        }
    }
    return nullptr;
}

static void acpi_init_madt()
{
  acpi.madt = (madt_t*)acpi_find_tbl("APIC");
  if(!acpi.madt) panic("acpi_init_madt() - no madt found");
  // Allocate memory for each table
    acpi.tbl.proc_apic = kmalloc(ACPI_APIC_TBL_SIZE);
    acpi.tbl.io_apic = kmalloc(ACPI_APIC_TBL_SIZE);
    acpi.tbl.ioso_apic = kmalloc(ACPI_APIC_TBL_SIZE);
    acpi.tbl.ionmi_apic = kmalloc(ACPI_APIC_TBL_SIZE);
    acpi.tbl.nmi_lapic = kmalloc(ACPI_APIC_TBL_SIZE);
    acpi.tbl.addro_lapic = kmalloc(ACPI_APIC_TBL_SIZE);
    acpi.tbl.procx2_lapic = kmalloc(ACPI_APIC_TBL_SIZE);

    
    /*
    // this was done in init_acpi when we memset acpi struct to 0
    acpi.idx.proc_apic = 0;
    acpi.idx.io_apic = 0;
    acpi.idx.ioso_apic = 0;
    acpi.idx.ionmi_apic = 0;
    acpi.idx.nmi_lapic = 0;
    acpi.idx.addro_lapic = 0;
    acpi.idx.procx2_lapic = 0;*/

    uint8_t* max_madt_addr = (uint8_t*)&acpi.madt->header + acpi.madt->header.length;
    uint8_t* ptr = (uint8_t*)&acpi.madt->table;
    while (ptr < max_madt_addr) {
        switch(*ptr) {             
            case PROCESSOR_LOCAL_APIC:
                acpi.tbl.proc_apic[acpi.idx.proc_apic++] = (proc_lapic_t*)ptr;
                break;
            case IO_APIC:
                acpi.tbl.io_apic[acpi.idx.io_apic++] = (io_apic_t*)ptr;
                break;
            case IO_APIC_INTERRUPT_SRC_OVERRIDE:
                acpi.tbl.ioso_apic[acpi.idx.ioso_apic++] = (ioso_apic_t*)ptr;
                break;
            case IO_APIC_NMI_SRC:
                acpi.tbl.ionmi_apic[acpi.idx.ionmi_apic++] = (ionmi_apic_t*)ptr;
                break;
            case LOCAL_APIC_NMI:
                acpi.tbl.nmi_lapic[acpi.idx.nmi_lapic++] = (nmi_lapic_t*)ptr;
                break;
            case LOCAL_APIC_ADDR_OVERRIDE:
                acpi.tbl.addro_lapic[acpi.idx.addro_lapic++] = (addro_lapic_t*)ptr;
                break;
            case PROCESSOR_LOCAL_X2_APIC:
                acpi.tbl.procx2_lapic[acpi.idx.procx2_lapic++] = (procx2_lapic_t*)ptr;
                break;
        }
        ptr += *(ptr + 1);
    }
    if(!acpi.debug_madt) return;

    debugf("== MADT Init Dump ==");
    debugf("  Processor Local APIC count: %lu", acpi.idx.proc_apic);
    debugf("  IO APIC count: %lu", acpi.idx.io_apic);
    debugf("  IO APIC Interrupt Source Override count: %lu", acpi.idx.ioso_apic);
    debugf("  IO APIC NMI Source count: %lu", acpi.idx.ionmi_apic);
    debugf("  Local APIC NMI count: %lu", acpi.idx.nmi_lapic);
    debugf("  Local APIC Address Override count: %lu", acpi.idx.addro_lapic);
    debugf("  Processor Local x2 APIC count: %lu", acpi.idx.procx2_lapic);
    debugf(" == end MADT dump ==\n\n"); 
}

void acpi_init(rsdp_t* rsdp)
{
  memset(&acpi, 0, sizeof(acpi_inst_t));
  if(!acpi_validate_rsdp(rsdp)) panic("ACPI - RSDP Invalid?!"); 
  debugf("rsdp = %lx", (uintptr_t)rsdp);
  acpi.rsdp = rsdp;
  acpi.is_v2 = rsdp->revision >= 2;

  acpi.rsdt = (rsdt_t*)((uintptr_t)rsdp->rsdt_addr);

  

  //just gonna id map it because we cant do anything elegant here (acpi rsdt ptr + higher half overflows!!!)
  virt_map_page(virt_current_pd(), (phys_addr_t)acpi.rsdt, acpi.rsdt, PAGE_DEFAULT)  ; //gotta map entire header :( 
  
  assert((phys_addr_t)acpi.rsdt->header.length < PAGE_SIZE);



  if(!acpi_validate_checksum(&acpi.rsdt->header) || !acpi_validate_sig(&acpi.rsdt->header, "RSDT")) panic("ACPI: invalid RSDT");

  debugf("rsdt found and validated at %lx", (uintptr_t)acpi.rsdt);

  acpi.debug_madt = true;
  acpi_init_madt();


}