#pragma once

#include <kstdlib.h>
#include <kernel/timer/timer.h>
#define CONFIG_REGISTER_OFFSET 0x10
#define INTERRUPT_STATUS_REGISTER_OFFSET 0x20
#define MAIN_COUNTER_REGISTER_OFFSET 0xf0

#define FEMTOSECONDS_PER_MICROSECOND 1000000000
#define FREQUENCY_HERTZ 1000
#define ENABLE_HPET 0x01
#define ENABLE_LEGACY_REPLACE 0x02
#define TIMER_ENABLE_INTERRUPT (1 << 2)
#define ENABLE_PERIODIC_MODE (1 << 3)
#define CHECK_PERIODIC_MODE (1 << 4)
#define ENABLE_SET_ACCUMULATOR (1 << 6)

typedef struct  {
    volatile uint64_t cap_config;
    volatile uint64_t comparator;
    volatile uint64_t fsb_int_route;
    volatile uint64_t reserved;
}  hpet_timer_t;

typedef struct {
    volatile uint64_t capabilities;
    volatile uint64_t reserved_0;
    volatile uint64_t config;
    volatile uint64_t reserved_1;
    volatile uint64_t interrupt_status;
    volatile uint64_t reserved_2[24];
    volatile char reserved_3;
    volatile uint64_t main_counter;
    volatile uint64_t reserved_4;
    volatile hpet_timer_t timers[];
}  hpet_table_t ;


typedef struct{
    volatile hpet_t* sdt;
    volatile hpet_table_t* tbl;

    uint64_t f_per_tick;
    uint64_t tick_per_us;
} hpet_inst_t;

extern hpet_inst_t HPET;

void hpet_init();
void hpet_timer_reset();
void hpet_timer_start();
timer_tick_t hpet_timer_poll_ticks();
femtosec_t hpet_timer_poll_time();


void usleep(microsec_t us);