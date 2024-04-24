#pragma once

#include "stdint.h"
#include "../common/scancode.h"
#include "../common/ascii.h"

uint8_t keyboard_get_lastevent();

 uint64_t keyboard_lastinput();



uint8_t keyboard_is_down(uint8_t sc);

static inline char keyboard_sc2ascii(uint8_t sc){
    return sc2ascii[sc];
}


static inline uint8_t keyboard_get_scancode(){ //abstract this away from os.h
     return (keyboard_get_lastevent() & 0x7f);
}
#define PS2_MAKE_MASK 0x80
static inline uint8_t keyboard_get_lastdown(){
    uint8_t sc = keyboard_get_lastevent();
    return (sc & PS2_MAKE_MASK) ? 0 : sc; //if msb = 1 key up
}