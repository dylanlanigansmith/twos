#pragma once
#include "../../kernel/stdlib/stdint.h"

typedef union { struct {
    uint8_t b,g,r,a;
} __attribute__((packed));
    uint32_t argb;
} color;
//so these are backwards, its argb when #ff00ff00


extern const color color_cyan;
extern const color color_red;
extern const color color_blue;
extern const color color_green;
extern const color color_white;
extern const color color_black;