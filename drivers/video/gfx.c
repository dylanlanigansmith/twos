#include "gfx.h"
#include "font8x8_basic.h"

#include "../../kernel/stdlib.h"




vec2 last_draw = {0,0}; 

void update_last_draw(const vec2 pos){
    last_draw = pos;
}
void reset_last_draw() { last_draw.x = GFX_TEXT_LEFT_MARGIN; last_draw.y =  GFX_TEXT_LINE_SPACING; }
/*
void __gfx_debug_v2(const vec2 dbg1, const vec2 dbg2){

    //this shows why yr gfx api has flaws
    vec2 ld = last_draw;
    gfx_clear_from_pos(clr_white, v2(0,100));
    const vec2 dbg_pos = {5, 150};
    gfx_print_at(itoa(dbg1.x, 10), dbg_pos);
    gfx_print_at(itoa(dbg1.y, 10), v2(dbg_pos.x + 5*FONT_W, dbg_pos.y));


    const vec2 dbg_pos2 = {5, 165};
    gfx_print_at(itoa(dbg2.x, 10), dbg_pos2);
    gfx_print_at(itoa(dbg2.y, 10), v2(dbg_pos2.x + 5*FONT_W, dbg_pos2.y));

    last_draw = ld;
}*/

gfx_state_t gfx_state;

void gfx_clear( const color clr)
{
     for (int w = 0; w < SCREEN_W; w++){
        for(int h = 0; h < SCREEN_H; h++){
            set_pixel(v2(w,h), clr );
        }
   }
   // memset((void*)(FB_ADDR), clr, FB_SIZE);
    gfx_state.clear_color = clr;
}

void gfx_init(const color clear_clr)
{

    GFX_STATE_SET_DEFAULTS(gfx_state);
    gfx_clear(clear_clr);
    reset_last_draw();
    gfx_state.has_init = True;
    oprintf(_COM,"Graphics Init %i x %i x %i \n", SCREEN_W, SCREEN_H, SCREEN_BPP * 8);
}

bool gfx_has_init()
{
    return gfx_state.has_init;
}

void gfx_fill_rect(const vec2 pos, const vec2 size, const color clr)
{
    for(int x = 0; x < size.x; ++x)
        for(int y = 0; y < size.y; ++y)
            set_pixel(v2(pos.x + x, pos.y + y), clr);
}

void gfw_fill_rect_fast(const vec2 pos, const vec2 size, const color clr)
{

}

void make_new_line(vec2* pos){
    
    pos->x = GFX_TEXT_LEFT_MARGIN;
    pos->y = pos->y + (FONT_H + GFX_TEXT_LINE_SPACING);

    if (pos->y > SCREEN_H)
            pos->y = 0; // bad bad bad
    
}

void gfx_draw_str(const char *str, vec2 pos, const color clr)
{
    for (int c = 0; str[c] != 0; ++c){
        if (str[c] == '\n'){
            make_new_line(&pos); continue;
        }
        
        gfx_draw_char(GET_FONT(str, c), pos, clr);
        pos.x += FONT_W;
        if (pos.x > (SCREEN_W - FONT_W + 2) ){
            make_new_line(&pos);
        }
       
    }
    update_last_draw(pos);
}



void gfx_draw_char(const char* font_char, const vec2 pos, const color clr)
{
    int cx, cy;
    int mask[8] = {1,2,4,8,16,32,64,128};
    for(cy = 0; cy < FONT_H; ++cy){
        for(cx=0; cx < FONT_W; ++cx){
            if(font_char[cy] & mask[cx]) set_pixel(v2(pos.x + cx, pos.y + cy + FONT_H), clr);
        }
    }

    update_last_draw(pos); //messsy
    last_draw.x += FONT_W; 

   
}

void gfx_print(const char *str)
{
    gfx_print_at(str, last_draw);
}

void gfx_print_at(const char *str, const vec2 pos)
{
    gfx_draw_str(str, pos, gfx_state.draw_color);
}

void gfx_printclr(const char *str, const color clr)
{
    gfx_draw_str(str, last_draw, clr);
}

void gfx_putc(const char c) //print single char (ex: from keyboard scancode)
{
    if(c == '\n'){
         make_new_line(&last_draw); return;
    }
    if (last_draw.x > (SCREEN_W - FONT_W) ){
            make_new_line(&last_draw);
    }
    gfx_draw_char(GFX_FONT[c], last_draw, gfx_state.draw_color);

}

void __gfx_fill_char(const vec2 pos){
    const static vec2 char_size = {FONT_W, FONT_H};
    gfx_fill_rect(pos, char_size, gfx_state.clear_color); //wrapper for if clear color is ever an issue
}

void gfx_delc() //remove last char
{
    vec2 pos = last_draw;

    //todo: not have one billion versions of this all doing same shit but brokenly
    if(pos.x - FONT_W >= SCREEN_W && pos.y > 0){
        pos.x = (SCREEN_W - GFX_TEXT_LEFT_MARGIN) ; //not right
        pos.y -= (FONT_H + GFX_TEXT_LINE_SPACING);
    }    
    pos.x -= FONT_W;
    
    
    //gfx_draw_char(GFX_FONT[' '], pos, gfx_state.draw_color);
   
    __gfx_fill_char(v2(pos.x, pos.y + FONT_H));
    update_last_draw(pos);

    
}
