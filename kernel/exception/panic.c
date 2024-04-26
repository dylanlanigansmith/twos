#include "../stdlib.h"

#include "panic.h"
#include "../stdio/stdout.h"

void post_panic(){
    __asm__ volatile ("cli;");

    stdout_force(1);

    __asm__ volatile (" mov rdi, 0xDEAD");
    for(;;)
        __asm__ volatile ("hlt");
}

void panic(const char *str)
{
    printf("Kernel Panic: %s");

    post_panic();
}

void panic_info(const char *str, const char* file, int line)
{
    printf("Kernel Panic: %s [%s](%i)", str, file, line);

    post_panic();
}

