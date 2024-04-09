#pragma once

/*
What Graphics Mode Is Set In Real Mode ?
*/

#define VGA_MODE_CHAR
//#define VGA_MODE_GFX

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