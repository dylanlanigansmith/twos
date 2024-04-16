#include "stdout.h"
#include "../mem/heap.h"

#include "../../drivers/video/gfx.h"
#include "../../drivers/input/keyboard.h"
stdout_t stdout; 


void stdout_clear(){
    stdout.index = 0; 
    memset(stdout.buffer, 0, stdout.size);
    stdout.buffer[stdout.index] = '>';
}

char* stdout_alloc(size_t size){
    char* tmp = kmalloc(size);
    if(!tmp) KPANIC("stdout buffer alloc fail");

    return tmp;
}

void stdout_lock(){
    if(stdout.lock == 0xff) KPANIC("attempt to lock stdout while locked");
    stdout.lock = 0xff;
}
void stdout_unlock(){
    stdout.lock = 0x00;
}

bool stdout_ready()
{
    return ((uintptr_t)stdout.buffer != 0 && stdout.size > 0);
}

uint8_t stdout_dirty()
{
    return stdout.dirty;
}

void stdout_flush()
{
    stdout.dirty = 0;
}

void stdout_init()
{
    stdout.lock = 0;
    stdout_lock();
    stdout.index = 0; 
    stdout.buffer =  stdout_alloc(STDOUT_BUFFER);
    stdout.size = STDOUT_BUFFER;
    stdout_clear();

   
    //set default flags
    stdout.flags |= stdout_backspace | stdout_wipeoverflow;
    stdout_unlock();

    oprintf(_COM,"\n=== stdout init @ %lx size %i idx %i  ready = %i ===\n", (uintptr_t)stdout.buffer, stdout.size, stdout.index, stdout_ready());
}

uint8_t stdout_update()
{
   
    if(!keys_available()) return 0;
    keys_lock_queue();
    uint8_t len = 0;
     char* keys = get_keys(&len); //stoopid
    
    for(int c = 0; keys[c] != 0; ++c) //redundant
        stdout_putchar(keys[c]);
    
    keys_reset_queue();

    return len;
}

void stdout_onoverflow(){
    stdout_lock();
    if( (stdout.flags & stdout_wipeoverflow)){

        stdout_clear();
    }
    else{
        size_t new_size = stdout.size + STDOUT_BUFFER;
        char* new_buf = stdout_alloc(new_size);         
        stdout.buffer = memcpy(new_buf, stdout.buffer, stdout.size);
        stdout.size = new_size;
        
    }
    stdout_unlock();
}

void stdout_putchar(uint8_t c)
{
    stdout.dirty = 1;
    //ASSERT(stdout.flags & stdout_backspace);
    if( (stdout.flags & stdout_backspace) || 1 ){
        //buffered term mode 
        switch (c)
        {
        
        case '\t':
            stdout_bytein(' '); stdout_bytein(' '); stdout_bytein(' '); stdout_bytein(' '); //python users triggered
            break;
        case '\b':
            //stdout_bytein(c);
           if(stdout.index) //dont go < 0
               stdout.index--; //do we want to just leave backspaces in and render them instead? //yeah
        //bug here sorta because gfx is handling backspaces live, we aint rewriting text every time
            break;
        default:
            stdout_bytein(c);
            break;
        }
    }
    else{
        stdout_bytein(c);
    }
}

void stdout_bytein(uint8_t byte)
{
    if(stdout.lock) return;

    if ( (stdout.index + 2) > stdout.size)
        stdout_onoverflow();
        //i am going fucking crazy
   // ASSERT(  stdout.index < stdout.size  );



    stdout.buffer[stdout.index++] = byte;
}

void stdout_set_updating(uint8_t val) //half brained way to know when we arent actually updating stdout 
{
    stdout.has_fb = val;
}

bool stdout_hasFB()
{
    return stdout.has_fb;
}
