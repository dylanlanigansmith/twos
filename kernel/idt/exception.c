#include "../../drivers/video/console.h"
#include "../pic/pic.h"
#include "../stdlib/string.h"
#include "../stdlib/string.h"

__attribute__((noreturn))
void exception_handler(void);

void exception_handler() {
    console_print("exception!");
    __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}


void key(){
    print("key");
}