#pragma once
#include "../../kernel/types/stdint.h"
#define VIDEO_MEM_ADDR 0xb8000

#define SCREEN_W 80
#define SCREEN_H 25


#define CLR_WHITEONBLK 0x0F


#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

struct screen_char_t {
    uint8_t chr;
    uint8_t prop;
};

void* get_video_memory() __attribute__((always_inline));
size_t get_video_memory_size() __attribute__((always_inline));


void clear_screen();

void* get_pos_addr(uint8_t col, uint8_t row); //w h 
void* get_cursor_addr();

void put_char_at(char c, uint8_t col, uint8_t row);

void put_char(char c);
void print_str(const char* str, uint8_t clr);

void print(const char* str);

uint16_t get_cursor();

int get_cursor_x();
int get_cursor_y();
void set_cursor(int x, int y);
void set_cursor_offset(uint16_t pos);

void disable_cursor();
void enable_cursor(uint8_t start, uint8_t end);