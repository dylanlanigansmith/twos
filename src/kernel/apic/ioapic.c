#include <kernel/kernel.h>
#include <kernel/printk.h>
#include <kernel/mem/virtmem.h>

#include <kernel/apic/ioapic.h>
//https://wiki.osdev.org/IOAPIC
//https://wiki.osdev.org/MADT
//https://pdos.csail.mit.edu/6.828/2014/readings/ia32/ioapic.pdf

struct {
    uint64_t idx;
    uintptr_t base;
    uint32_t gsi_base;
    uint8_t max_redir;
} ioapic;

uint32_t ioapic_read(uint32_t reg) {
     *(volatile uint32_t*)(ioapic.base ) = (volatile uint32_t)reg;
    return *(volatile uint32_t*)(ioapic.base + 0x10ull);
}

void ioapic_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)(ioapic.base ) = (volatile uint32_t)reg;
    *(volatile uint32_t*)(ioapic.base + 0x10ull) = (volatile uint32_t)data;
}

uint64_t ioapic_read_redtbl_entry(uint8_t entry) {
    uint8_t offset = IOAPICREDTBL(entry);
    uint64_t res = ioapic_read(offset);
    res |= (ioapic_read(offset + 1) << 31);
    return res;
}

int ioapic_mask_gsi(uint8_t idx) {
    uint64_t data = ioapic_read_redtbl_entry(idx);
    if (idx > ioapic.max_redir) return 1;
        
    
    uint8_t offset = IOAPICREDTBL(idx);
    ioapic_write(offset, data | (1 << 16));
    return 0;
}
int ioapic_unmask_gsi(uint8_t idx) {
    uint64_t data = ioapic_read_redtbl_entry(idx);
    if (idx > ioapic.max_redir) return 1;

    uint8_t offset = IOAPICREDTBL(idx);
    data &= ~(1 << 16);
    ioapic_write(offset, data);
    return 0;
}

void ioapic_mask_all(){
    for (int i = IDT_EXCEPTIONS; i < IDT_EXCEPTIONS + NUM_IRQ; i++) 
        ioapic_mask(i);
}

void ioapic_unmask_all(){
    for (int i = IDT_EXCEPTIONS; i < IDT_EXCEPTIONS + NUM_IRQ; i++) 
        ioapic_unmask(i);
}


void ioapic_mask(uint8_t irq) {
     if (irq == 0) {
      //  *lvt_timer_reg |= (1 << 16);
    } else {
        for (uint64_t i = 0; i < acpi.idx.ioso_apic; i++) {
            if (acpi.tbl.ioso_apic[i]->irq == irq) {
                 ioapic_mask_gsi((uint8_t)acpi.tbl.ioso_apic[i]->gsi);
                return;
            }
        }
    }
    ioapic_mask_gsi(irq);
}
void ioapic_unmask(uint8_t irq) {
    if (irq == 0) {
       // *lvt_timer_reg &= ~(1 << 16);
    } else {
        for (uint64_t i = 0; i < acpi.idx.ioso_apic; i++) {
            if (acpi.tbl.ioso_apic[i]->irq == irq) {
                 ioapic_unmask_gsi((uint8_t)acpi.tbl.ioso_apic[i]->gsi);
                return;
            }
        }
    }
    ioapic_unmask_gsi(irq);
}

void ioapic_map_redtbl_entry(uint8_t entry, uint8_t vector) {
    union redirection_entry_t data;

    data.raw_data = ioapic_read_redtbl_entry(entry);
    data.entry.vector = vector + IDT_EXCEPTIONS;
    data.entry.mask = 1;
    data.entry.destination = 0;

    uint8_t offset = IOAPICREDTBL(entry);
    ioapic_write(offset, data.registers.lower);
    ioapic_write(offset + 1, data.registers.upper);
}

int irq_map[NUM_IRQ] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
void ioapic_init(){
    memset(&ioapic, 0, sizeof(ioapic) );
    memset(irq_map, 0, sizeof(int) * NUM_IRQ);


    ioapic.base = ((uintptr_t)acpi.tbl.io_apic[0]->io_apic_addr);
    ioapic.gsi_base = acpi.tbl.io_apic[0]->gsib;
    
    assert(ioapic.base + KERNEL_VADDR < (KERNEL_VADDR + KERNEL_VADDR_SIZE)); 
    virt_map_page(virt_current_pd(), ioapic.base, ioapic.base, PAGE_DEFAULT);


    ioapic.max_redir = (uint8_t)( (ioapic_read(IOAPICVER) >> 16)+ 1u)  ;

    debugf("ioapic {%lx} max=%u", ioapic.base, ioapic.max_redir)

    for (uint8_t i = 0; i < ioapic.max_redir + 1; i++) 
        ioapic_mask_gsi(i);
    
     for (uint64_t i = 0; i < acpi.idx.ioso_apic; i++) {
        ioapic_map_redtbl_entry(acpi.tbl.ioso_apic[i]->gsi, acpi.tbl.ioso_apic[i]->irq);
        if (acpi.tbl.ioso_apic[i]->gsi < 16) {
            irq_map[acpi.tbl.ioso_apic[i]->gsi] = 1;
        }
    }

    for (uint8_t i = 0; i < NUM_IRQ; i++) {
        if (!irq_map[i]) {
            ioapic_map_redtbl_entry(i, i);
        }
    }
    


}