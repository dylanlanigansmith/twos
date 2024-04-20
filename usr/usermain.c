#include "../lib/ustdlib.h"



void main(){
    const char* str = "hello from user mode.";
     const char* str2 = "please mr kernel can I print";
    
    print(str);
    print(str);
    print(str2);

    print("/n");

    print(str2);


    
}

void _start(){
  
    main();
    for(;;) { __asm__ volatile ("mov rax, 0xcafebabe; hlt"); }
}
