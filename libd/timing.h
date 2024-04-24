#pragma once
#include "stdint.h"

#define TICK_HZ 1000

#define MS_PER_TICK 1000   //(1/TICK_HZ) * 1000

uint64_t get_ms_since_boot();


static inline uint64_t tick_to_ms(uint64_t tick){
    //so like since we are using "1000hz"
    //it should be 1:1 ignoring emulation errors
    //so uh..
    return tick; //nice..

}


void sleep_ms(uint64_t ms);