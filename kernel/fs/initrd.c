
#include "initrd.h"

initrd_header* header;
initrd_footer* footer;


int initrd(uintptr_t addr, size_t size)
{
    //TODO: check if addr has virtual/id map!!!!!
        //bochs also loaded at 0x123000 so multiboot might be blessin us up
    header = footer = 0;
    if (!addr || ! size){
        debugf("initrd: failed! invalid address, invalid size"); return 1;
    }   

    debugf("===initrd: loading %lu bytes from %lx===\n", size, addr);

    header = (initrd_header*)(addr);

    if (header->crc != INITRD_CRC){ debugf("initrd: header CRC INVALID! \n"); return 1; }
    debugf("initrd: header CRC ok! \n");
    debugf("initrd: header:\n %u files, %u dirs\n", header->num_files, header->num_dirs);

    footer = (initrd_footer*)(addr + size - sizeof(initrd_footer));
    if (footer->crc != INITRD_CRC){ debugf("initrd: footer CRC INVALID! \n"); return 1; }
    debugf("initrd: footer CRC ok! \n");
    debugf("initrd: footer: total ramdisk size = %lu \n", footer->size); //redundant but makes me happy

    initrd_dir_info* dirs = (initrd_dir_info*)(  addr + sizeof(initrd_header) );
    debugf("initrd: processing directories:\n");
    for(int i = 0; i < header->num_dirs; ++i){
        printf("    found dir: %s [%u] CRC=%i\n", dirs[i].name, dirs[i].dir_inode, (dirs[i].crc == INITRD_CRC));
    }

    initrd_file_header* files[header->num_files];
    size_t offset = addr + sizeof(initrd_header) + (sizeof(initrd_dir_info) * header->num_dirs);
    
    size_t total_file_size = 0;
    debugf("initrd: processing files:\n");
    for(int i = 0; i < header->num_files; ++i){
        files[i] = (initrd_file_header*)(offset);
        if(files[i]->crc != INITRD_CRC) { debugf("initrd: fileheader[%i] CRC INVALID! \n", i); return 1; }
        
        debugf("    found file at +%lx! %s [%lu bytes], dir[%u]\n", files[i]->offset, files[i]->name, files[i]->size, files[i]->parent_dir);
        total_file_size += files[i]->size;
        offset += (sizeof(initrd_file_header) + files[i]->size);
    }

    debugf("initrd validated; contains %lu bytes of files!", total_file_size);
   
    /*
        now we have to make a VFS ah
    */

    debugf("==initrd load success!== \n\n");
    return 0;
}