#include "../stdlib/stdint.h"
#include "../../drivers/video/console.h"
#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;




/*__attribute__((noreturn)) void __stack_chk_fail(void)
{
 clear_screen();
 print("stack check fail");
    for(;;) {__asm__ volatile("nop");}

    __builtin_unreachable();
}*/
