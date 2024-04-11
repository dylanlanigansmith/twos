#include "task.h"


#define DEBUGT(fmt, ...) printf(fmt, __VA_ARGS__)

int tasking_init(){
    DEBUGT("%s", "tasking init");

    return 0;
}

int fork(){
    return 0;
}

void yield(){

}

PID_t getpid(){
    return -1;
}