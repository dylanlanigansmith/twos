#include <kernel/kernel.h>
#include <kernel/apic/apic.h>
#include <kernel/apic/pic.h>
#include <kernel/printk.h>
#include <kernel/cpuid.h>
#include <kernel/mem/virtmem.h>


extern void cpuid_proc_feat_info(cpuid_registers_t* regs);

static bool apic_is_available() {
    cpuid_registers_t regs;
    cpuid_proc_feat_info(&regs);
    return regs.edx & CPUID_FEAT_EDX_APIC;
}

struct {
    uint64_t id;
    phys_addr_t base;
} lapic;

void lapic_write(uint32_t reg, uint32_t data) {
    *((volatile uint32_t*)(lapic.base + reg)) = data;
}

uint32_t lapic_read(uint32_t reg) {
    return *((volatile uint32_t*)(lapic.base + reg));
}




uint8_t get_lapic_id()
{
    return (uint8_t)lapic_read(LAPIC_ID_REG);
}

static void lapic_enable() {
    lapic_write(APIC_SPURIOUS_VECTOR_REG, APIC_ENABLE | SPURIOUS_INT);
}
void apic_init()
{
    assert(apic_is_available());
    memset(&lapic, 0, sizeof(lapic) );

    lapic.base = (phys_addr_t)acpi.madt->lapic_addr;
    lapic.id = acpi.tbl.proc_apic[0]->apic_id; //this doesnt change right
    virt_map_page(virt_current_pd(), lapic.base, lapic.base, PAGE_DEFAULT);
    debugf("enabled LAPIC {%lx}[%lu]", lapic.base, lapic.id)

    PIC_remap(PIC1_REMAP_OFFSET, PIC2_REMAP_OFFSET);
    PIC_disable();
    lapic_enable();
    
}