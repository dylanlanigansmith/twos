#pragma once
#include "../stdlib/stdint.h"

#define TIMER_CLOCK_IN 1193180

#define TIMER_CMD 0x43
#define TIMER_REG0 0x40
#define TIMER_REG1 0x41
#define TIMER_REG2 0x42

#define TIMER_MODE_REPEAT 0x36


#define PIT_RATE 400 //it hertz

void timer_init(uint32_t freq);

extern uint64_t tick;