#pragma once
#include "stdint.h"

static inline int get_highest_bit(uint32_t num) { //32 
    if (num == 0) return 0; 
    int position = sizeof(num) * 8 - 1; 
    while (!(num & (1u << position))) {
        position--; 
    }
    return position;
}

