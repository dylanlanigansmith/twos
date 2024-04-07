#include "../../drivers/video/video.h"
#include "../pic/pic.h"
#include "../types/string.h"


__attribute__((noreturn))
void exception_handler(void);

void exception_handler() {
    print("exception!");
    __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}


void key(){
    print("key");
}