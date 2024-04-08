#include "print.h"

#include "../../drivers/video/gfx.h"
#include "../../drivers/video/console.h"

void print(const char* str){
    #ifdef VGA_MODE_GFX
        gfx_print(str);
    #else
        console_print(str)
    #endif
}


//we should have a bullet proof print for errors etc