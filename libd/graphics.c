#include "graphics.h"
#include "stdlib.h"
#include "os.h"
#include "assert.h"


gfx_info_t* gfxinfo = 0;

void* gfx_load_info(gfx_info_t* gi){
    uint64_t p = (uint64_t)gi;
     uint64_t fb = 0;
    __asm__ volatile("mov rdi, %1; mov rax, 19; int 0x69; mov %0, rax; " : "=r"(fb) : "r"(p) : "rax", "rdi");

    return (void*)fb;
}


gfx_info_t *gfx_get_info()
{
    if(gfxinfo && gfxinfo->fb && gfxinfo->h && gfxinfo->w){
        //we def got it 
        return gfxinfo;
    }
    
    gfx_info_t* gfxinfo = malloc(sizeof(gfx_info_t)); //yk really could just save this once and return it 
    assert(gfxinfo);

    void* fbc = gfx_load_info(gfxinfo);
    if(!fbc){
        printf("failed to get gfxinfo!");
        return 0;
    }

    assert((uintptr_t)fbc == gfxinfo->fb); //poor mans checksum

    printf("{ [%ix%i@%i] 0x%lx/%ip }\n", gfxinfo->w, gfxinfo->h, gfxinfo->bpp, gfxinfo->fb, gfxinfo->p);
    return gfxinfo;
}


void *set_gfx_mode()
{
     print("set gfx mode!");
     uint64_t ret = 0;
     __asm__ volatile ( "mov rax, 7; int 0x69; mov %0, rax; " : "=r"(ret) ::  "rax") ;

    if(gfxinfo){
        //we already have gfxinfo so we can see if fb is the same & toggle mode
        //in future might return current mode here instead..
    }
     return (void*)ret;
}
void *gfx_clear(uint32_t argb)
{
    uint64_t idiotproof = ((uint64_t)argb << 32) | (uint64_t)argb ;
    return syscall(6, idiotproof); //probs doesnt work
}