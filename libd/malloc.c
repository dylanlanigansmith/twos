#include "stdint.h"
#include "os.h"
#include "stdlib.h"


struct malloc_state_t{
    uintptr_t addr;
    size_t size;

} malloc_state;


void _init_heap(){

}


void* malloc(size_t size)
{
    return 0;
}

void free(void* p){
    
}

void *calloc(size_t num, size_t size)
{
    return nullptr;
}

void *realloc(void *ptr, size_t new_size)
{
    return nullptr;
}
