#pragma once
#include <kernel/acpi/acpi.h>


#define IO_WIN_ADDR 0xfec00010
#define IO_REG_SEL_ADDR 0xfec00000

#define IOAPICID 0x00
#define IOAPICVER 0x01
#define IOAPICARB 0x02
#define IOREDTBL_BASE 0x03

#define IDT_EXCEPTIONS 32
#define NUM_IRQ 16


#define IOAPICREDTBL(n)   (0x10 + 2 * (uint8_t)n )

struct redirection_entry_regs {
    uint32_t lower;
    uint32_t upper;
}__attribute__((packed));

struct redirection_entry {
    uint64_t vector        : 8;
    uint64_t delivery_m    : 3;
    uint64_t destination_m : 1;
    uint64_t status_m      : 1;
    uint64_t polarity      : 1;
    uint64_t irr           : 1;
    uint64_t trigger       : 1;
    uint64_t mask          : 1;
    uint64_t reserved      : 39;
    uint64_t destination   : 8;
}__attribute__((packed));

union redirection_entry_t {
    struct redirection_entry entry;
    struct redirection_entry_regs registers;
    volatile uint64_t raw_data;
}__attribute__((packed));

void ioapic_init();
void ioapic_unmask(uint8_t irq);
void ioapic_mask(uint8_t irq);
void ioapic_mask_all();
void ioapic_unmask_all();


