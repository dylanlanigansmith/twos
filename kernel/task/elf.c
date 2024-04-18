#include "elf.h"
/*
Info and structs from: https://uclibc.org/docs/elf-64-gen.pdf


also this https://stackoverflow.com/a/71367851 made everything clear
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

    //go thru program headers
    //for now we are just trying to find an entry point 
    uintptr_t entry = 0;

    for(int i = 0; i < eheader->e_phnum; ++i){
        Elf64_Phdr* phdr = (file->addr + eheader->e_phoff + (sizeof( Elf64_Phdr) * i)  );

        if(phdr->p_type == PT_LOAD){

            if(phdr->p_flags & PF_X){
                debugf("found prog header for executable, loadable segment [%i] offset %lx, vm %lx,  size file: %lx \n", i, phdr->p_offset, phdr->p_vaddr,phdr->p_filesz);

                if(phdr->p_vaddr <= eheader->e_entry && eheader->e_entry <= phdr->p_vaddr + phdr->p_memsz){

                    entry = (eheader->e_entry - phdr->p_vaddr) + phdr->p_offset;
                    debugf("entry point found, %lx from start of file\n", entry);

                    break;
                }


            }
        }

    }


    /*
    well shit

    we gotta rewrite everything LOL

    so to load an executable as a process
    parse elf headers, figure out what is wanted at what virt addrs, etc.
        -basically there
    
    then we have to make p4,p3,p2 for entire damn process

    then we have to modify scheduler a tonne
        -seperate user and kernel tasks

    and theres no real good way to test this

    isnt that fun!?

    
    deps:
        finding physical memory

        somewhere to put pages

        scheduler cr3 fixin
    

    aghhghgghghghghghghhghghg
    */
    
    

   
    return entry;
}