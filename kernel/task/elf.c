#include "elf.h"
/*
Info and structs from: https://uclibc.org/docs/elf-64-gen.pdf

*/

uint8_t elf_verify_magic(Elf64_Ehdr* eheader){
    if(eheader) //isnt it beautiful?
        if(eheader->e_ident[0] == 0x7f)
            if(eheader->e_ident[1] == 'E')
                if(eheader->e_ident[2] == 'L')
                    if(eheader->e_ident[3] == 'F')
                        return 1;
    return 0;
}


uint64_t elf_fail(const char* msg){
    debugf("load_elf failure: %s \n", msg);
    return 0;
}

uint64_t load_elf(vfs_node* file)
{
    Elf64_Ehdr* eheader = (Elf64_Ehdr* )(file->addr);

    if(!elf_verify_magic(eheader)){
        debugf("failed to verify file is elf64 format\n"); return 0;
    }

    debugf("file %s is valid elf64. loading..\n", file->name);

    if(eheader->e_ident[EI_DATA] != ELFDATA2LSB) return elf_fail("not little endian");
    if(eheader->e_ident[EI_OSABI] != ELFOSABI_SYSTEMV) return elf_fail("not SystemV ABI");
    if(eheader->e_ident[EI_CLASS] != ELFCLASS64) return elf_fail("not ELF64!");
    
    if(eheader->e_type != ET_EXEC) return elf_fail("not an executable! no shared lib support yet");



    debugf("entry pt is at %lx, file %lx, offset %lx",(uintptr_t)file->addr + eheader->e_entry, (uintptr_t)file->addr, eheader->e_entry);

    Elf64_Phdr* phdr = (file->addr + eheader->e_phoff);

    

    uintptr_t entry = 0;
    return entry;
}