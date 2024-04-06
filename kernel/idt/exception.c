#include "../../drivers/video/video.h"
#include "../pic/pic.h"
#include "../types/string.h"


__attribute__((noreturn))
void exception_handler(void);

void exception_handler() {
    uint16_t irr = pic_get_irr();
    print(itoa(irr, 10));
    print("ex");
    __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}


void key(){
    print("key");
}