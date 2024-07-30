#pragma once
#include <kernel/kernel.h>
#include <kernel/acpi/acpi.h>
#include <kernel/printk.h>

#include <kernel/mem/virtmem.h>
#include <kernel/isr/isr.h>

#include <kernel/timer/hpet.h>

hpet_inst_t HPET;



void hpet_timer_reset() { HPET.tbl->main_counter = 0ull;}
void hpet_timer_start() { HPET.tbl->config |= ENABLE_HPET; }
uint64_t hpet_timer_poll_ticks() {return HPET.tbl->main_counter; }
uint64_t hpet_timer_poll_time() { return HPET.tbl->main_counter * HPET.f_per_tick; }




void hpet_init()
{
    memset(&HPET, 0, sizeof(HPET));

    HPET.sdt = (hpet_t*)(acpi_find_tbl("HPET"));
    if(!HPET.sdt) panic("No HPET Found");

   
    virt_map_page(virt_current_pd(), HPET.sdt, HPET.sdt, PAGE_DEFAULT);

    HPET.tbl = (volatile hpet_table_t*)HPET.sdt->address_info.address;
     debugf("found hpet @ %lx tbl @ %lx", HPET.sdt, HPET.tbl);
    HPET.f_per_tick = HPET.tbl->capabilities >> 32;
    HPET.tick_per_us =FEMTOSECONDS_PER_MICROSECOND / HPET.f_per_tick; 
    
    HPET.tbl->config  &= ~(ENABLE_LEGACY_REPLACE | ENABLE_HPET);
  
     // The following code is useful for periodic timer that triggers interrupts on IRQ 0
    /*hpet_table->config |= ENABLE_LEGACY_REPLACE;
    if (!(hpet_table->config & ENABLE_HPET)) {
        hpet_table->main_counter = 0;
    }
    hpet_table->timers[0].cap_config |= (TIMER_ENABLE_INTERRUPT | ENABLE_PERIODIC_MODE | ENABLE_SET_ACCUMULATOR);
    hpet_table->timers[0].comparator = frequency / FREQUENCY_HERTZ;
    hpet_table->config |= ENABLE_HPET;
    unmask_irq(TIMER_IRQ);*/
}

void usleep(microsec_t us)
{
    hpet_timer_reset();
    hpet_timer_start();

    while ( hpet_timer_poll_time() < ((uint64_t)FEMTOSECONDS_PER_MICROSECOND * us)) { }
    return;
}