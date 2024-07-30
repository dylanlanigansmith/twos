#include <kernel/panic.h>
#include <kernel/printk.h>

 [[noreturn]] void panic(const char* msg){
    debugf(msg); //shout into the void!
    for (;;)
        __asm__("hlt");
    __builtin_unreachable();
}