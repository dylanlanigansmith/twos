#pragma once
#include "stdint.h"


typedef struct{
    uint64_t fb;
    uint16_t w,h,p;
    uint8_t bpp;
    uint8_t curmode;
} __attribute__((packed)) gfx_info_t;


#define VGA_MODE_GFX

//#define VGA_COLOR_16 
#ifdef VGA_COLOR_16

typedef uint8_t vga_clr;

enum vga_colors {
    clr_black = 0,
    clr_blue,
    clr_green,
    clr_cyan,
    clr_red,
    clr_magenta,
    clr_brown,
    clr_gray,
    clr_gray_dark,
    clr_lblue,
    clr_lgreen,
    clr_lcyan,
    clr_lred,
    clr_lmagenta,
    clr_yellow,
    clr_white
};

#endif