#include "elf.h"
#include "../mem/page.h"
#include "../mem/vas.h"
#include "../pmm/pmm.h"
#include "../fs/initrd.h"
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

Elf64_Shdr* find_section(uintptr_t base, Elf64_Ehdr* eheader, Elf64_Word type, uint32_t idx) //finds FIRST section be warned
{
    if(idx > 0){
        Elf64_Shdr* shdr = (base + eheader->e_shoff + (sizeof( Elf64_Shdr) * idx)  );
        //maybe check type maybe not
        if (shdr->sh_type != type) 
            debugf("returning a section not of requested type, (want %li found %li), good luck bud", type, shdr->sh_type);
        return shdr;
    }

    for(uint32_t  i = 1; i < eheader->e_shnum; ++i){
        Elf64_Shdr* shdr = (base + eheader->e_shoff + (sizeof( Elf64_Shdr) * i)  );
        if (shdr->sh_type == type)
            return shdr;
    }
    return 0;
}

Elf64_Shdr* shdr_from_idx(uintptr_t base, Elf64_Ehdr* eheader,int idx){
    return (Elf64_Shdr* )(base + eheader->e_shoff + (sizeof( Elf64_Shdr) * idx)  );
}


Elf64_Phdr* find_phdr(uintptr_t base, Elf64_Ehdr* eheader, Elf64_Word type){
    for(int i = 0; i < eheader->e_phnum; ++i){
        Elf64_Phdr* phdr = (base + eheader->e_phoff + (sizeof( Elf64_Phdr) * i)  );
        if(phdr->p_type == type)
            return phdr;
     }

     return 0;
}

Elf64_Ehdr* validate_elf_file(vfs_node* file)
{
    if(!file) {
        debugf("cannot validate null elf file"); return 0;
    }
    Elf64_Ehdr* eheader = (Elf64_Ehdr* )(file->addr);

    if(!elf_verify_magic(eheader)) {
        debugf("failed to verify file %s is elf64 format\n", file->name); return 0;

        return 0;

    }
    if(eheader->e_ident[EI_DATA] != ELFDATA2LSB) return elf_fail("not little endian");
    if(eheader->e_ident[EI_OSABI] != ELFOSABI_SYSTEMV) return elf_fail("not SystemV ABI");
    if(eheader->e_ident[EI_CLASS] != ELFCLASS64) return elf_fail("not ELF64!");
    
    if(eheader->e_type != ET_EXEC && eheader->e_type != ET_DYN) return elf_fail("not an executable or shared lib");
    return eheader;
}


typedef struct 
{
    uintptr_t low,high;
}elf_vaddr_t;

elf_vaddr_t elf_find_psec_range(uintptr_t base, Elf64_Ehdr* eheader)
{
    debugf("elf_find_psec_range [%i] \n", eheader->e_phnum);
    uintptr_t vaddr_low = UINT64_MAX;
    uintptr_t vaddr_high = 0;
    for(int i = 0; i < eheader->e_phnum; ++i){
        Elf64_Phdr* phdr = (base + eheader->e_phoff + (sizeof( Elf64_Phdr) * i)  );
         debugf("phdr found type %i \n", phdr->p_type);
        if(phdr->p_type == PT_LOAD ){
            if(phdr->p_vaddr < vaddr_low) vaddr_low = phdr->p_vaddr;
            if(phdr->p_vaddr > vaddr_high) vaddr_high = phdr->p_vaddr;
            if(phdr->p_vaddr + phdr->p_memsz > vaddr_high) vaddr_high = phdr->p_vaddr + phdr->p_memsz; //highest section + its size

            debugf("phdr load  vaddr = %lx, aligned to %lx, %s %s %s  \n", phdr->p_vaddr, phdr->p_align, (phdr->p_flags & PF_X) ? "X" : "", (phdr->p_flags & PF_W) ? "W" : "",(phdr->p_flags & PF_R) ? "R" : ""); 
            if(phdr->p_flags & PF_X){
            
            }
        }

    }
    elf_vaddr_t ret = {vaddr_low, vaddr_high};
    debugf("----\n");
    return ret;
}



uintptr_t load_elf(vfs_node* file, user_vas_t* usr)
{
    Elf64_Ehdr* eheader = validate_elf_file(file);
    if(!eheader) return 0;

    debugf("file %s is valid elf64. loading..\n", file->name);

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


    //we should just stick these in with the loaded binary
    //map kernel 

    size_t vas_size = size_paligned + PAGE_SIZE; //add a page for now just to stick our extra shit in 
    uintptr_t phys = pmm_alloc(vas_size); 



    ASSERT(phys);
    //we really dont need a whole extra page btw 
    uintptr_t phys_end = phys + vas_size;
    
    uintptr_t v_end =  vaddr_low + vas_size;
    
    

    ASSERT(map_phys_addr(vaddr_low, phys,vas_size, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER));

     usr->pt.p4p = (phys_end - sizeof(page_table_t) * 3); //all currently physical
     usr->pt.p3p = (phys_end - sizeof(page_table_t) * 2);
     usr->pt.p2p = (phys_end - sizeof(page_table_t) );

     usr->pt.p4 = (page_table_t*)(v_end - sizeof(page_table_t) * 3); //virtuals
     usr->pt.p3 = (page_table_t*)(v_end - sizeof(page_table_t) * 2);
     usr->pt.p2 = (page_table_t*)(v_end - sizeof(page_table_t) );

    debugf("allocated user pages: [phys address] =  p4 %lx p3 %lx p2 %lx\n [virt] vstart = %lx vend = %lx, size palign %lx size total %lx", 
        usr->pt.p4p,usr->pt.p3p,usr->pt.p2p,vaddr_low, v_end, size_paligned,    vas_size );
    memset((void*)(v_end - (sizeof(page_table_t) * 3) ), 0, sizeof(page_table_t) * 3);


    map_user_page_tables(vaddr_low, phys, vas_size, &usr->pt);
  
    usr->size = vas_size;
    usr->entry = eheader->e_entry;
    usr->phys = phys;
    usr->vaddr.h = vaddr_high;
    usr->vaddr.l = vaddr_low;
    usr->vaddr.len = vaddr_high - vaddr_low;
    usr->stack.top = (uintptr_t)usr->pt.p2; 

    if(usr->stack.top % 0x10 > 0){
        usr->stack.top =- (usr->stack.top % 0x10); //align to 16 bytes 
    }
    usr->stack.bot = vaddr_high + 0x20; //idk make a lil safety for weird bugs to slip thru the cracks with

    debugf("usr stack @ %lx, size = %li kb", usr->stack.top, BYTES_TO_KIB( (usr->stack.top - usr->stack.bot) ));

    
    
    //load elf into phys mem somewhere
    
   
    
   

  

    for(int i = 0; i < eheader->e_phnum; ++i){
        Elf64_Phdr* phdr = (file->addr + eheader->e_phoff + (sizeof( Elf64_Phdr) * i)  );

        if(phdr->p_type != PT_LOAD || !phdr->p_vaddr) continue;
        
        uintptr_t dest = phdr->p_vaddr ;
        debugf("copying header %lx to %lx. fs= %lx ms = %lx", phdr->p_vaddr, dest, phdr->p_filesz, phdr->p_memsz);
        memcpy((void*)dest, (file->addr + phdr->p_offset), phdr->p_filesz); //size might be wrong
    }
    
    


    // add user tasks to task.c, fix cr3, send it
    
    //also should unmap pages 
    if(unmap_phys_addr(vaddr_low, vas_size))
        debugf("failed to unmap vaddr pages ?\n");

    
    


    
    debugf("loaded ELF: %s [%li kb]", file->name, usr->vaddr.len);
    

   
    return 0;
}




//fuck this we arent doing it
uintptr_t load_elf_so(vfs_node* file, user_vas_t* usr)
{
    Elf64_Ehdr* eheader = validate_elf_file(file);
    if(!eheader) return 0;

      
    uintptr_t base = file->addr;
    
    Elf64_Shdr* secstrhdr = find_section(base, eheader, SHT_STRTAB, eheader->e_shstrndx);
    ASSERT(secstrhdr );
    char* strtbl_sec = (char*)(base + secstrhdr->sh_offset);
    

    debugf("Sections in ELF64 File %s:", file->name);
     for(int i = 0; i < eheader->e_shnum; ++i){
        Elf64_Shdr* shdr = (file->addr + eheader->e_shoff + (sizeof( Elf64_Shdr) * i)  );
        const char* name = (char*)(strtbl_sec + shdr->sh_name);
        debugf("    %s (%li): size(%li) va = %lx, %li aligned \n",name, shdr->sh_type,shdr->sh_size, shdr->sh_addr, shdr->sh_addralign);
    } 
    
   
   
    //writing this assuming we know we need to load 1 library from here on
    //can make better once it works

    

    vfs_node* so = 0;

    Elf64_Phdr* dynhdr = find_phdr(base, eheader, PT_DYNAMIC);
    if(dynhdr)
    {
        Elf64_Shdr* adynhdr = find_section(base, eheader, SHT_DYNAMIC,0);
        ASSERT( adynhdr); //lazy
        Elf64_Shdr* dynstrhdr = find_section(base, eheader, SHT_STRTAB, adynhdr->sh_link);
        ASSERT(dynstrhdr);
        char* strtbl_dyn = (char*)(base + dynstrhdr->sh_offset);
        if(strtbl_dyn[0] != 0) debugf("elf dynamic strtbl seems funky\n");

        Elf64_Dyn* dyn =  (Elf64_Dyn*)(base + dynhdr->p_offset);
        int i = 0;
        while( dyn[i].d_tag != DT_NULL )
        {
            if( dyn[i].d_tag == DT_NEEDED){
                const char* name = (strtbl_dyn + dyn[i].d_un.d_val);
                debugf("loading NEEDED LIB %s\n", name);
                so = initrd_findfile(initrd_root, name);
                if(!so) debugf("couldnt find %s\n", name);
            }


            i++;
        }
        debugf("processed %i entries in .dynamic \n", i);
    }
    
    

    Elf64_Ehdr* lehdr = validate_elf_file(so);
    ASSERT(lehdr);
    
    //load this elf 

    elf_vaddr_t laddr = elf_find_psec_range(so->addr, lehdr);
    
    size_t libsize_real = laddr.high - laddr.low;
    size_t libsize = round_up_to_page(libsize_real);
    debugf("LIB %s vaddrL= %lx, vaddrH= %lx size %li ps %lx \n",so->name, laddr.low, laddr.high, libsize_real, libsize);


    uintptr_t vaddr_low = UINT64_MAX;
    uintptr_t vaddr_high = 0;
    for(int i = 0; i < eheader->e_phnum; ++i){
        Elf64_Phdr* phdr = (base + eheader->e_phoff + (sizeof( Elf64_Phdr) * i)  );
        debugf("phdr type %li found\n", phdr->p_type);
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


   

    size_t vas_size = size_paligned + PAGE_SIZE + libsize; //add a page for now just to stick our extra shit in, then another page for lib
    uintptr_t phys = pmm_alloc(vas_size); 



    ASSERT(phys);
    //we really dont need a whole extra page btw 
    uintptr_t phys_end = phys + vas_size;
    
    uintptr_t v_end =  vaddr_low + vas_size;
    
    

    ASSERT(map_phys_addr(vaddr_low, phys,vas_size, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER)); //map prog
    ASSERT(map_phys_addr(laddr.low, phys_end - libsize,vas_size, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER)); //map so

     usr->pt.p4p = (phys_end - sizeof(page_table_t) * 3); //all currently physical
     usr->pt.p3p = (phys_end - sizeof(page_table_t) * 2);
     usr->pt.p2p = (phys_end - sizeof(page_table_t) );

     usr->pt.p4 = (page_table_t*)(v_end - sizeof(page_table_t) * 3); //virtuals
     usr->pt.p3 = (page_table_t*)(v_end - sizeof(page_table_t) * 2);
     usr->pt.p2 = (page_table_t*)(v_end - sizeof(page_table_t) );

    debugf("allocated user pages: [phys address] =  p4 %lx p3 %lx p2 %lx\n [virt] vstart = %lx vend = %lx, size palign %lx size total %lx", 
        usr->pt.p4p,usr->pt.p3p,usr->pt.p2p,vaddr_low, v_end, size_paligned,    vas_size );
    memset((void*)(v_end - (sizeof(page_table_t) * 3) ), 0, sizeof(page_table_t) * 3);




    map_user_page_tables(vaddr_low, phys, vas_size, &usr->pt); //map main program
  


    map_user_page_tables(laddr.low, phys_end - libsize, libsize, &usr->pt); //map library



    usr->size = vas_size;
    usr->entry = eheader->e_entry;
    usr->phys = phys;
    usr->vaddr.h = vaddr_high;
    usr->vaddr.l = vaddr_low;
    usr->vaddr.len = vaddr_high - vaddr_low;
    usr->stack.top = (uintptr_t)usr->pt.p2; 

    if(usr->stack.top % 0x10 > 0){
        usr->stack.top =- (usr->stack.top % 0x10); //align to 16 bytes 
    }
    usr->stack.bot = vaddr_high + 0x20; //idk make a lil safety for weird bugs to slip thru the cracks with

    debugf("usr stack @ %lx, size = %li kb", usr->stack.top, BYTES_TO_KIB( (usr->stack.top - usr->stack.bot) ));

    
    
    //load elf into phys mem somewhere
    
    for(int i = 0; i < eheader->e_phnum; ++i){
        Elf64_Phdr* phdr = (file->addr + eheader->e_phoff + (sizeof( Elf64_Phdr) * i)  );

        if(phdr->p_type != PT_LOAD || !phdr->p_vaddr) continue;
        
        uintptr_t dest = phdr->p_vaddr ;
        debugf("copying header %lx to %lx. fs= %lx ms = %lx", phdr->p_vaddr, dest, phdr->p_filesz, phdr->p_memsz);
        memcpy((void*)dest, (file->addr + phdr->p_offset), phdr->p_filesz); //size might be wrong
    }
    debugf("copied program\n");
    //load our library as well
    for(int i = 0; i < lehdr->e_phnum; ++i){
        Elf64_Phdr* phdr = (so->addr + lehdr->e_phoff + (sizeof( Elf64_Phdr) * i)  );

        if(phdr->p_type != PT_LOAD || !phdr->p_vaddr) continue;
        
        uintptr_t dest = phdr->p_vaddr ;
        debugf("copying header %lx to %lx. fs= %lx ms = %lx", phdr->p_vaddr, dest, phdr->p_filesz, phdr->p_memsz);
        memcpy((void*)dest, (so->addr  + phdr->p_offset), phdr->p_filesz); //size might be wrong
    }

     debugf("copied shared library\n");

    //now we fix relocates?
    //idk




    // add user tasks to task.c, fix cr3, send it
    //we should also unmap pages now
    

    
    


    
    debugf("loaded ELF: %s [%li kb]", file->name, usr->vaddr.len);
    

   
    return 0;
}