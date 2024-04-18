#include "../lib/ustdlib.h"



void main(){
    const char* str = "hello from user mode.";
     const char* str2 = "please mr kernel can I print";
    for(int i = 0; i < 1000; ++i){
        if(i == 999){
            print(str);
        }
    }

    print("/n");

    print(str2);


    syscall(2, 0); //exit(0)
}

void _start(){
    main();
    
}
