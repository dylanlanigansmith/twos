#include "stdout.h"
#include "../mem/heap.h"
#include "../sys/sysinfo.h"
#include "../../drivers/video/gfx.h"
#include "../../drivers/input/keyboard.h"
stdout_t stdout; 


void stdout_clear(){

    stdout.index = 0; 
    memset(stdout.buffer, 0, stdout.size);
    stdout.buffer[stdout.index] = '.';
    stdout.lines = 0;
    stdout.dirty = 1;

    gfx_clear(gfx_state.clear_color); //hate it
}

void stdout_force(int clr) //itrps should b off
{
    if(!stdout_ready()) return;
    if(clr == 0)
        gfx_clear_text();
    else
        gfx_clear(color_red);
   // PIC_disable();
    gfx_print(get_stdout());
    stdout_flush();
   // PIC_enable();
    
            
}

char* stdout_alloc(size_t size){
    char* tmp = kmalloc(size);
    if(!tmp) KPANIC("stdout buffer alloc fail");

    return tmp;
}

void stdout_free(char* buf){
    void* p = (void*)buf;
    debugf("stdout_free( %lx )\n", p);
    kfree(p);
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
    stdout.lines = 0;
   
    //set default flags
    stdout.flags |= (stdout_backspace | stdout_wipeoverflow);
    stdout_unlock();

    oprintf(_COM,"\n=== stdout init @ %lx size %i idx %i  ready = %i ===\n", (uintptr_t)stdout.buffer, stdout.size, stdout.index, stdout_ready());
}


int find_idx_after_lines(int lines){
    // we should really treat width as a line here too on the gfx side but :/
    int i = 0,found = 0;

    const char* buf = get_stdout();
    while(buf[i] != '\0'){
        if(buf[i] == '\n'){
          //  debugf("found \\n @ %i/%i %i", found, lines, i);
            found++;
        } 
        if(found >= lines)
            return i;
        i++;
    }
    ;
    return 0;
}

uint8_t stdout_safescroll(uint8_t lines) //broken
{
   
    int to_remove =  (lines / 5); //remove 20% of lines
   
    int idx = find_idx_after_lines(to_remove);
     debugf("scrolling stdout! %i to remove, before idx %i, at index %i \n", to_remove, idx, stdout.index);
    if(idx == 0 || idx >= (stdout.index - 1 ) || 1){ //could be all short \ns
        stdout_clear(); return 0;
    }
    stdout_lock();
    char* new_buf = stdout_alloc(STDOUT_BUFFER); 
     ASSERT(new_buf);        
     memset(new_buf, 0, STDOUT_BUFFER );
   

    size_t curend, cursize, curoff, newend, newsize;
    curoff = stdout.buffer + idx;
    curend = stdout.buffer + stdout.index;
    cursize = curend - curoff;

    debugf("copying from %lx to %lx (%lx) (to new at %lx) \n", curoff, curend, cursize, new_buf);
    new_buf = memcpy(new_buf, curoff, cursize);
   
    newend = new_buf + cursize;
     stdout.size = STDOUT_BUFFER; //should already be
   //   debugf("settingfrom %lx to %lx (%lx) (to new)\n", new_buf + (stdout.index - idx),(new_buf + (stdout.index - idx)) + (stdout.index - (stdout.index - idx)),  stdout.index - (stdout.index - idx));
   

    stdout_free(stdout.buffer);
    stdout.buffer = new_buf;
     debugf("len now: %li", strlen(stdout.buffer));
    stdout.index = strlen(stdout.buffer);
    stdout.lines -= to_remove;
    stdout_unlock();
    gfx_clear(gfx_state.clear_color);
    //stdout.dirty = 1;
    //return number of lines removed 
    return 0;
}

uint8_t stdout_update() //i think u can trash this
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
  //  stdout.dirty = 1;
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
       
        case '\0':
            break; //this is likely bad added like 4 weeks after i wrote this so probably not needed
        case '\n':
            stdout.lines++;
        default:
            stdout_bytein(c);
            break;
        }
    }
    else{
        stdout_bytein(c);
    }
   
    if(stdout.lines > STDOUT_MAXLINES){
       stdout_safescroll(stdout.lines);
    }
    stdout.dirty = 1;
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
{                                       //hey! full brain here! before you add this shitty code, why not just make stdout_display_update() a function!? 
    stdout.has_fb = val;
}

bool stdout_hasFB() //see stdout_set_updating
{
    return stdout.has_fb;
}
