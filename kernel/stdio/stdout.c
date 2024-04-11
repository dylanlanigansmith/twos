#include "stdout.h"
#include "../mem/heap.h"

#include "../../drivers/video/gfx.h"
stdout_t stdout; 


void stdout_clear(){
    stdout.index = 0; 
    memset(stdout.buffer, 0, stdout.size);
    stdout.buffer[stdout.index] = '>';
}

char* stdout_alloc(size_t size){
    char* tmp = kmalloc(size);
    if(!tmp) panic("stdout buffer alloc fail");

    return tmp;
}

void stdout_lock(){
    if(stdout.lock == 0xff) panic("attempt to lock stdout while locked");
    stdout.lock = 0xff;
}
void stdout_unlock(){
    stdout.lock = 0x00;
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

     oprintf(_COM,"\n=== stdout init @ %lx size %i idx %i ===\n", (uintptr_t)stdout.buffer, stdout.size, stdout.index);
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
    if( (stdout.flags & stdout_backspace) ){
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
    stdout.buffer[stdout.index++] = byte;


}
