#pragma once
#include "../../kernel/stdlib/stdint.h"
#include "video.h"
#define VIDEO_MEM_ADDR 0xb8000

#define CONSOLE_W 80
#define CONSOLE_H 25


#define CLR_WHITEONBLK 0x0F
#define CLR

#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5



static inline uint8_t get_color(uint8_t fg, uint8_t bg){ //for text
    return (bg << 4) | (fg & 0x0f);
}

struct screen_char_t {
    uint8_t chr;
    uint8_t prop;
};

void* get_console_memory() __attribute__((always_inline));
size_t get_console_memory_size() __attribute__((always_inline));


void clear_screen();

void* get_pos_addr(uint8_t col, uint8_t row); //w h 
void* get_cursor_addr();

void console_put_char_at(char c, uint8_t col, uint8_t row);

void put_char(char c);
void console_print_str(const char* str, uint8_t clr);

void console_print(const char* str);

uint16_t get_cursor();

int get_cursor_x();
int get_cursor_y();
void set_cursor(int x, int y);
void set_cursor_offset(uint16_t pos);

void disable_cursor();
void enable_cursor(uint8_t start, uint8_t end);