#include "libd.h"


//extern main(int c, char** v);
extern void _init_heap(size_t sz);
void _libd_init(){
    _init_heap(HEAP_INITIAL_SIZE);
}

void _libd_destroy(){ //to be called by exit!
   //we would manage restoring states and stuff here like gfx mode and stdout stuff 
}