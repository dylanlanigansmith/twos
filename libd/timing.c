#include "timing.h"
#include "os.h"



uint64_t get_ms_since_boot()
{
    register uint64_t tick = sys_gettick();
    return tick_to_ms(tick); //compiler better optimize this out
}

void sleep_ms(uint64_t ms)
{
    sys_sleepms(ms);

    __asm__ volatile ("nop; nop; nop; nop; nop; nop; nop; nop;"); //make sure we are back & let pending itrps hopefully fire
    yield(); //trust issues
     __asm__ volatile ("nop; nop; nop; nop; nop; nop; nop; nop;");
}
