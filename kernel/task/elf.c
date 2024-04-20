#include "elf.h"
#include "../mem/page.h"
#include "../mem/vas.h"
#include "../pmm/pmm.h"
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

uint64_t find_raw_entry(vfs_node* file, Elf64_Ehdr* eheader){ //useless
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
    return entry;
}

uintptr_t load_elf(vfs_node* file, user_vas_t* usr)
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
/*
    for(int i = 0; i < eheader->e_shnum; ++i){
        Elf64_Shdr* shdr = (file->addr + eheader->e_shoff + (sizeof( Elf64_Shdr) * i)  );
        debugf("section type %li vaddr = %lx, aligned to %li \n", shdr->sh_type, shdr->sh_addr, shdr->sh_addralign);
    } */

    uintptr_t vaddr_low = UINT64_MAX;
    uintptr_t vaddr_high = 0;
    for(int i = 0; i < eheader->e_phnum; ++i){
        Elf64_Phdr* phdr = (file->addr + eheader->e_phoff + (sizeof( Elf64_Phdr) * i)  );

        if(phdr->p_type == PT_LOAD && phdr->p_vaddr){
            if(phdr->p_vaddr < vaddr_low) vaddr_low = phdr->p_vaddr;
            if(phdr->p_vaddr > vaddr_high) vaddr_high = phdr->p_vaddr;
            if(phdr->p_vaddr + phdr->p_memsz > vaddr_high) vaddr_high = phdr->p_vaddr + phdr->p_memsz; //highest section + its size

             debugf("phdr load  vaddr = %lx, aligned to %lx, %s %s %s  \n", phdr->p_vaddr, phdr->p_align, (phdr->p_flags & PF_X) ? "X" : "", (phdr->p_flags & PF_W) ? "W" : "",(phdr->p_flags & PF_R) ? "R" : ""); 
            if(phdr->p_flags & PF_X){
            
            }
        }

    }
    size_t size_paligned = round_up_to_page(vaddr_high - vaddr_low);
    debugf("lowest vaddr %lx highest vaddr %lx, size %lx", vaddr_low, vaddr_high, size_paligned);

    //so we need to make a whole ass page table

   // page_table_t* p4 = user_palloc();
   // page_table_t* p3 = user_palloc();
  //  page_table_t* p2 = user_palloc();
   // debugf("making user pages: p4 %lx p3 %lx p2 %lx", p4,p3,p2);
   
    //load elf into phys mem somewhere
    
    uintptr_t phys = pmm_alloc(size_paligned);
    
    ASSERT(map_phys_addr(vaddr_low, phys, size_paligned, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER));

  //  map_user_page_tables(vaddr_low, phys, size_paligned, p4,p3,p2);

    for(int i = 0; i < eheader->e_phnum; ++i){
        Elf64_Phdr* phdr = (file->addr + eheader->e_phoff + (sizeof( Elf64_Phdr) * i)  );

        if(phdr->p_type != PT_LOAD || !phdr->p_vaddr) continue;
        
        uintptr_t dest = phdr->p_vaddr ;
        debugf("copying header %lx to %lx. fs= %lx ms = %lx", phdr->p_vaddr, dest, phdr->p_filesz, phdr->p_memsz);
        memcpy((void*)dest, (file->addr + phdr->p_offset), phdr->p_filesz); //size might be wrong
    }
    
    usr->entry = eheader->e_entry;
    usr->phys = phys;
   // usr->pt.p2 = p2;
   // usr->pt.p3 = p3;
   // usr->pt.p3 = p4;
    usr->vaddr.h = vaddr_high;
    usr->vaddr.l = vaddr_low;
    usr->stack.top = vaddr_low + size_paligned - (0x10 * 4); //stupid but temporary
    usr->stack.bot = vaddr_high + 0x100;


    // add user tasks to task.c, fix cr3, send it
    
    

    
    


    
    
    

   
    return 0;
}