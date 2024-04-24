#pragma once
#include "../stdlib/stdint.h"

#define TIMER_CLOCK_IN 1193180

#define TIMER_CMD 0x43
#define TIMER_REG0 0x40
#define TIMER_REG1 0x41
#define TIMER_REG2 0x42

#define TIMER_MODE_REPEAT 0x36


#define PIT_RATE 1000 //it hertz
//also in libd/time.h


void timer_init(uint32_t freq);

extern uint64_t tick;


typedef struct {
    uint64_t ns_since;

} ktime_t;


static inline void ktime_create(ktime_t* kt){
    kt->ns_since = 0;
    
}