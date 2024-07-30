#pragma once
#include <kstdlib.h>


// helper function to convert HSV to RGB without floating pt arith for test pattern
static inline void HSVtoRGB(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
    uint8_t region, remainder, p, q, t;

    if (s == 0) {
        // If saturation is 0, the color is a shade of gray
        *r = *g = *b = v;
        return;
    }
    // 43 = 256 / 6 (for 6 regions of the color wheel)
    region = h / 43;
    remainder = (h - (region * 43)) * 6;
    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
    switch (region) {
        case 0:
            *r = v; *g = t; *b = p;
            break;
        case 1:
            *r = q; *g = v; *b = p;
            break;
        case 2:
            *r = p; *g = v; *b = t;
            break;
        case 3:
            *r = p; *g = q; *b = v;
            break;
        case 4:
            *r = t; *g = p; *b = v;
            break;
        case 5:
        default:
            *r = v; *g = p; *b = q;
            break;
    }
}



static inline void framebuffer_display_test_pattern(uint32_t *framebuffer, uint32_t width, uint32_t height, uint32_t pitch){
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint16_t hue = (uint16_t)((uint32_t)x * 256 / width); // 0 to 255
            uint8_t r, g, b;
            HSVtoRGB(hue, 255, 255, &r, &g, &b);
            uint32_t color = (r << 16) | (g << 8) | b; //havent ported fb driver yet
            framebuffer[y * (pitch / 4) + x] = color;
        }
    }
}
