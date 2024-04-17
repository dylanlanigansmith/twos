#include "vfs.h"

vfs_node* vfs_root = 0;

vfs_node *init_vfs(uintptr_t ptr, size_t size) //unused 
{
    return nullptr;
}





void vfs_printfile(vfs_node *node, size_t len)
{
    if(!node) return;

    if(len > node->length || !len) len = node->length;
    //this needs to be all standardized with io abstraction 
    //no standardized put byte >:(
        //i added one but its just moving the problem

    size_t idx = 0;
    while(idx < len){
        char dumb = *(char*)( node->addr + idx++);
        if(dumb == 0) return;

        putc(dumb); //UGH
    }
    
}
void vfs_printfile_at(vfs_node *node, size_t offset, size_t len)
{
    if(!node) return;

    if(offset + len > node->length || !len) len = node->length - offset;
    //this needs to be all standardized with io abstraction 
    //no standardized put byte >:(
        //i added one but its just moving the problem

    size_t idx = offset;
    while (idx < (offset + len) ){
        char dumb = *(char*)( node->addr + idx++);
        if(dumb == 0) return;

        putc(dumb); //UGH
    }
    
}
