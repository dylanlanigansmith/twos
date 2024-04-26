#include "console.h"
#include "../../kernel/stdlib/stdint.h"
#include "../../kernel/stdlib/memory.h"

#include "../port/port.h"

//https://wiki.osdev.org/VGA_Fonts


/*

#ifdef VGA_MODE_CHAR
#define VGA_MODE_CHECK()  ;
#define VGA_MODE_CHECK_RET() ; 
#else
#define VGA_MODE_CHECK() return;
#define VGA_MODE_CHECK_RET() return 0;
#endif

//this is all for text mode 

void* get_console_memory(){
    return (void*)VIDEO_MEM_ADDR;
}
size_t get_console_memory_size() {
    static const size_t vid_mem_size = 2*(CONSOLE_H*CONSOLE_H + CONSOLE_W); //0x582
    return vid_mem_size;
}

void clear_screen(){ //todo add color
    VGA_MODE_CHECK();
    memset(get_console_memory(), 0, get_console_memory_size());
}

void* get_pos_addr(uint8_t col, uint8_t row)
{
    //if(row > CONSOLE_H || col > CONSOLE_W)
    //    return VIDEO_MEM_ADDR;
    return (void*) (VIDEO_MEM_ADDR + 2 * (row * CONSOLE_W + col));
}
void* get_cursor_addr()
{
 return (void*)(VIDEO_MEM_ADDR + get_cursor());   
}


void console_put_char_at(char c, uint8_t col, uint8_t row)
{   
    VGA_MODE_CHECK();
    char* cell = (char*)get_pos_addr(col, row);
    *cell = c;
    *(cell + 0x1) = CLR_WHITEONBLK;
}

void put_char(char c)
{
    VGA_MODE_CHECK();
    uint16_t cursor = get_cursor();
    if(c == '\n'){
        uint16_t cursor_row = get_cursor_y();
        set_cursor(CONSOLE_W, cursor_row);
        return;
    }
    char* cell = (char*)get_cursor_addr();
    *cell = c;
    *(cell + 0x1) = CLR_WHITEONBLK;
    set_cursor_offset(get_cursor() + 0x2);
}



void console_print_str(const char* str, uint8_t clr){
    VGA_MODE_CHECK();
    int len = 0;
    for(int i = 0; str[i] != 0; ++i){
        put_char(str[i]);
        len = i;
    }
}

void console_print(const char* str)
{
    VGA_MODE_CHECK();
    console_print_str(str,0);
}

uint16_t get_cursor(){
    port_byte_out(REG_SCREEN_CTRL, 0x0F); //use ctrl reg to select internal reg, 15 = high byte of cursor offset ?
    uint16_t offset = 0;
    offset |= port_byte_in(REG_SCREEN_DATA);
    port_byte_out(REG_SCREEN_CTRL, 0x0E); // select low byte of cursor offset ?
    offset |= port_byte_in(REG_SCREEN_DATA) << 8;

    return offset;
}

int get_cursor_x()
{
    return (int)(get_cursor() / 2) % CONSOLE_W;
}

int get_cursor_y()
{
    return (int)(get_cursor() / 2) / CONSOLE_W;
}

void set_cursor(int x, int y)
{
    VGA_MODE_CHECK();
    uint16_t pos = 2 * (y * CONSOLE_W + x);
    set_cursor_offset(pos);
    
}

void set_cursor_offset(uint16_t pos)
{
    VGA_MODE_CHECK();
    port_byte_out(REG_SCREEN_CTRL, 0x0F);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)(pos & 0xFF)); //write high ?
    port_byte_out(REG_SCREEN_CTRL, 0x0E);
    port_byte_out(REG_SCREEN_DATA, (uint8_t)((pos >> 8) & 0xFF)); //write low
}

void disable_cursor()
{
    VGA_MODE_CHECK();
    port_byte_out(REG_SCREEN_CTRL, 0x0A); 
    port_byte_out(REG_SCREEN_DATA, 0x20);
}

void enable_cursor(uint8_t start, uint8_t end) //set the start and end scanlines (min 0 max 15?)
{ //https://wiki.osdev.org/Text_Mode_Cursor#Moving_the_Cursor_2

    VGA_MODE_CHECK();
    if(!start && !end){
        start = 0; end = CONSOLE_H; //should maybe be screen height
    }
    port_byte_out(REG_SCREEN_CTRL, 0x0A); 
    port_byte_out(REG_SCREEN_DATA, (port_byte_in(REG_SCREEN_DATA) & 0xC0) | start );

    port_byte_out(REG_SCREEN_CTRL, 0x0B);
    port_byte_out(REG_SCREEN_DATA, (port_byte_in(REG_SCREEN_DATA) & 0xE0) | end);

}

*/