#pragma once
#include "../../kernel/stdlib.h"
#include "color.h"
#include "../../common/video.h"
#include "../../kernel/sys/sysinfo.h"
#define SCREEN_W sysinfo.fb.w
#define SCREEN_H sysinfo.fb.h
#define SCREEN_PITCH sysinfo.fb.pitch
#define SCREEN_BPP 4 //bytes


#define FB_PHYS_ADDR sysinfo.fb.addr
#define FB_ADDR sysinfo.fb.addr
//#define FB_PHYS_ADDR 0xfc000000
//#define FB_ADDR 0xfc000000
#define FB_BPP 32
#define FB_SIZE (size_t)(SCREEN_H * SCREEN_PITCH)
#define FB_END_ADDR  (uintptr_t)(FB_ADDR + FB_SIZE)

typedef struct{
    uint32_t x,y;
} vec2; 
static inline vec2 v2(const uint32_t x, const uint32_t y) { vec2 p = {x,y}; return p; }







#define GFX_FONT font8x8_basic
#define GET_FONT(str, chr) GFX_FONT[str[chr]]

#define GFX_TEXT_CLR_DEFAULT clr_black
#define GFX_TEXT_LEFT_MARGIN 2
#define GFX_TEXT_LINE_SPACING 2

#define GFX_CLEAR_CLR_DEFAULT clr_white

typedef struct {
    bool has_init;
    color draw_color;
    color clear_color;

    uint8_t mode; 

    gfx_info_t info;
    
} gfx_state_t; 

//this is where my lack of clarity re: c vs cpp best practices is shown


extern gfx_state_t gfx_state;


static inline void gfx_set_color(gfx_state_t* gs, const color clr) { gs->draw_color = clr; }
static inline void gfx_set_default_color(gfx_state_t* gs, const color clr) { *(uint32_t*)(&(gs->draw_color)) = 0xffffffff; } //oh god

static inline uint32_t* get_pixel(const vec2 pos){
    return (uint32_t*)( FB_ADDR + SCREEN_PITCH * pos.y + pos.x * SCREEN_BPP);
}

static inline void set_pixel(const vec2 pos, const color clr){
    color* pixel = get_pixel(pos);
    *pixel = clr;
}

static inline void gfx_get_info(gfx_info_t* gi){
    if(!gi) return 0;
    gi->fb = gfx_state.info.fb;
    gi->w = gfx_state.info.w;
    gi->h = gfx_state.info.h;
    gi->p = gfx_state.info.p;
    gi->bpp = gfx_state.info.bpp;
    gi->curmode = gfx_state.mode;
}


void gfx_clear( const color clr);

void gfx_clear_text(); 

void gfx_clear_line(uint32_t y, uint32_t height); 

void gfx_clear_to_pos(const color clr, const vec2 pos);
void gfx_clear_from_pos(const color clr, const vec2 pos);

static inline void gfx_clear_white(){
     memset((void*)FB_ADDR, 0xff, FB_SIZE);
}

void gfx_init(const color clear_clr);

int gfx_has_init(); // swear to god
//drawing

void gfx_fill_rect(const vec2 pos, const vec2 size, const color clr);
//void gfw_fill_rect_fast(const vec2 pos, const vec2 size, const vga_clr clr);

//character stufg
//both assume transparent bg 
void gfx_draw_str(const char* str, vec2 pos, const color clr);
void gfx_draw_char(const char* font_char, const vec2 pos, const color clr);

void reset_last_draw();
void gfx_draw_char_at(const char* font_char, const vec2 pos, const color clr);

void gfx_print(const char* str);
void gfx_print_at(const char* str, const vec2 pos);
void gfx_printclr(const char* str, const color clr); 

void gfx_print_pos(const char* str, vec2 pos);

void gfx_putc(const char c);
void gfx_delc();