#include "keyboard.h"

#include "../../kernel/stdlib/stdint.h"
#include "../../kernel/stdlib/memory.h"
#include "../../kernel/idt/isr.h"
#include "../port/port.h"
#include "../video/gfx.h"
#include "../video/console.h"

#include "../../common/scancode.h"
#include "../../common/ascii.h"
#include "../../kernel/stdio/stdout.h"
#include "keyevent.h"
#include "../../kernel/timer/timer.h"


uint8_t shouldScan = 1;

//0x2a down 0xaa up
uint8_t lshift = 0;
#define KEY_QUEUE_SIZE 64
uint8_t key_queue[64];
uint8_t key_idx = 0;


uint8_t keys[0xff] = {0};

uint64_t last_key_event = 0 ;
KeyEventStream keyevent;


int keymode = 0;

static void keydown_isr(registers_t* regs){
    if(!shouldScan) return;
    uint8_t scan = port_byte_in(0x60); 
    if(scan == 0 || scan == 0xff || scan == 0x9d) return;
    if(scan >= 254) return;
    //unknown re: 0x9d ^
    last_key_event = tick;
    if ( (scan & PS2_MAKEORBREAK) ){
        //break code, key up
        uint8_t rscan = scan & 0x7f; //clear msb
        keys[rscan] = False;
       // printf("%s UP", get_scancode_name(scan));
    } else {
        //make code, key down
        keys[scan] = True;
        //printf("%s DOWN", get_scancode_name(scan));
    }
   
    keystream_add(&keyevent, scan); 


/*
    if(scan == SCAN_LSHIFT_DOWN) lshift = 1;
    else if(scan == SCAN_LSHIFT_UP) lshift = 0;
   
    
    //gfx_print(htoa(sc2ascii[scan]));


    char c = sc2ascii[scan];
    //our old kernel level code 
    if(c != 0x0){
        if(lshift)
           c = apply_shift_modifier(c);
        
        if(key_idx >= KEY_QUEUE_SIZE)
            key_idx = 0;
        key_queue[key_idx] = c;
        key_idx++;
        stdout_putchar(c);
    }
    */

    

}


void keyboard_init()
{
    register_interupt_handler(IRQ1, (isr_t)&keydown_isr);
    key_idx = 0;
    memset(key_queue, 0, KEY_QUEUE_SIZE);

    memset((void*)keys, 0, sizeof(keys));

    init_keystream(&keyevent);
}

uint8_t is_key_down(uint8_t sc)
{
    return keys[sc];
}
uint8_t keys_last_event()
{
    return keystream_latest(&keyevent);
}

uint8_t keys_getqueued()
{
    return keystream_getandremove(&keyevent);
}

void keys_gamemode()
{
    if(keymode == 0){
        port_byte_out(0x60, 0xf8); keymode = 1;
    } else{
        port_byte_out(0x60, 0xf6); keymode = 0;
    }
}

uint8_t is_key_down_ascii(char c)
{
    //this is not to be actually used!
    //youll see why
  
    char nc = ascii_normalize_alphanum(c);
    if(nc == 0) return False;

    for(uint8_t i  = 0; i < 0x3A; ++i) //random guess on where ascii scancodes end
        if(sc2ascii[i] == nc)
            return keys[i];
    

    return False;
}






uint8_t* get_keys(uint8_t* amt)
{
    
    uint8_t len = 0;
   // print(key_queue);
    while(key_queue[len] != 0){
        
        len++;
    }
    *amt = len;
    return key_queue;
}

bool keys_available()
{
    return key_idx > 0;
}

void keys_lock_queue()
{
    __asm__ volatile ("cli;");
    
}

void keys_reset_queue()
{
    key_idx = 0;
    __asm__ volatile ("sti;");
}
