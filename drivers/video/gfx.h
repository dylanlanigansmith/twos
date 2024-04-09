#pragma once
#include "../../kernel/stdlib.h"

#include "video.h"

#define SCREEN_W 1024
#define SCREEN_H 768

#define FB_ADDR 0xfc000000 
#define FB_BPP 32
#define FB_SIZE (size_t)(SCREEN_W * SCREEN_H + SCREEN_W) * 4 
#define FB_END_ADDR (FB_ADDR + FB_SIZE)

typedef struct{
    uint16_t x,y;
} vec2; 
static inline vec2 v2(const uint16_t x, const uint16_t y) { vec2 p = {x,y}; return p; }



#define GFX_FONT font8x8_basic
#define GET_FONT(str, chr) GFX_FONT[str[chr]]

#define GFX_TEXT_CLR_DEFAULT clr_black
#define GFX_TEXT_LEFT_MARGIN 2
#define GFX_TEXT_LINE_SPACING 2

#define GFX_CLEAR_CLR_DEFAULT clr_white

typedef struct {
    bool has_init;
    vga_clr draw_color;
    vga_clr clear_color;
} gfx_state_t; 

//this is where my lack of clarity re: c vs cpp best practices is shown
#define GFX_STATE_SET_DEFAULTS(gfx) gfx.has_init=False; gfx.draw_color=GFX_TEXT_CLR_DEFAULT; gfx.clear_color=GFX_CLEAR_CLR_DEFAULT;

extern gfx_state_t gfx_state;


static inline void gfx_set_color(gfx_state_t* gs, const vga_clr clr) { gs->draw_color = clr; }
static inline void gfx_set_default_color(gfx_state_t* gs, const vga_clr clr) { gs->draw_color = GFX_TEXT_CLR_DEFAULT; }

static inline uint8_t* get_pixel(const vec2 pos){
    return (uint8_t*)( FB_ADDR + SCREEN_W * pos.y + pos.x);
}

static inline void set_pixel(const vec2 pos, const vga_clr clr){
    uint8_t* pixel = get_pixel(pos);
    *pixel = clr;
}


void gfx_clear( const vga_clr clr);

static inline void gfx_clear_from_pos(const vga_clr clr, const vec2 pos){
    void* addr = (void*)(get_pixel(pos));

    memset(addr, clr, FB_END_ADDR - (size_t)addr);
}

void gfx_init(const vga_clr clear_clr);

//drawing

void gfx_fill_rect(const vec2 pos, const vec2 size, const vga_clr clr);
//void gfw_fill_rect_fast(const vec2 pos, const vec2 size, const vga_clr clr);

//character stufg
//both assume transparent bg 
void gfx_draw_str(const char* str, vec2 pos, const vga_clr clr);
void gfx_draw_char(const char* font_char, const vec2 pos, const vga_clr clr);

void gfx_print(const char* str);
void gfx_print_at(const char* str, const vec2 pos);
void gfx_printclr(const char* str, const vga_clr clr); 

void gfx_putc(const char c);
void gfx_delc();