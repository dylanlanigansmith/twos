#include "page.h"
#include "heap.h"
#include "../../drivers/serial/serial.h"
#include "../../kernel/sys/sysinfo.h"
#include "page_alloc.h"

#include "../pmm/pmm.h"

#include "vas.h"

extern uint64_t get_cr3();

extern uintptr_t p2_table;
extern uintptr_t p3_table;
extern uintptr_t p4_table;


page_table_t* get_p4(){
    return (page_table_t*)(get_cr3());
}

void flush_tlb(){
    
    //static page_table_t p4_wtf; 
    //memset(&p4_wtf, 0, 512*8);

    //uint64_t ptr = &p4_table;
   // __asm__ volatile ("movq rax, cr3; movq cr3, (%0); movq cr3, rax" : : "r"(ptr));
     __asm__ volatile ("movq rax, cr3; movq cr3, rax; nop; movq rax, cr3; movq cr3, rax;");
    serial_println("==flushed TLB==");

}

void invalidate_page(uintptr_t virtual_addr){
    __asm__ volatile ("mov rax, (%0); invlpg [rax]" : : "r"(virtual_addr)); 
    serial_printh("invalidated", virtual_addr);
    //https://forum.osdev.org/viewtopic.php?t=24676 masm=intel bug
}

typedef struct {
    uintptr_t p4, pdpt3, pd2, pt;
} page_indices_t;

void get_page_index_vm(uint64_t virtual_addr, page_indices_t* idx){
    debugf("idx for VA: %lx [%lb] ", virtual_addr, virtual_addr);
    idx->p4 = (virtual_addr >> 39) & 0x1FF;
    idx->pdpt3 = (virtual_addr >> 30) & 0x1FF;
    idx->pd2 = (virtual_addr >> 21) & 0x1FF;
    idx->pt = (virtual_addr >> 12) & 0x1FF; //always 0
    debugf(" p4[%lu] p3[%lu] p2[%lu] p1[%lu] \n", idx->p4, idx->pdpt3, idx->pd2, idx->pt);
}



#define P2_SHIFT 21


#define PT_FLAGS 0b11llu




void dump_pt(page_table_t* p, const char* str, bool skip0){
     for(int i = 0; i < 512; ++i){
        
        if(skip0 && p->entries[i] == 0)
            continue;
        serial_print(itoa(i,10));
        serial_print(" > ");
        serial_printh(str, p->entries[i]);
    }
}


void map_identity(page_table_t* p2, page_table_t* p4, uint64_t addr, int num_pages){
    ASSERT(is_page_aligned(addr));
    
    page_indices_t ids = {0,0,0,0};
    get_page_index_vm(addr, &ids);

    page_table_t* p3 = pt_addr(p4->entries[ids.p4]);
    debugf("map_identity( p2 = %lx p3 = %lx p4 = %lx addr = %lx num = %i)", (uint64_t)p2, (uint64_t)p3, (uint64_t)p4, addr, num_pages);
    for(uint64_t i = 0; i < num_pages; ++i){
        
        uint64_t current_virtual_address = addr + (i * PAGE_SIZE);
        uint64_t current_physical_address = addr + (i * PAGE_SIZE); 
        
        int idx = (current_virtual_address >> P2_SHIFT) & 0x1ff;
        p2->entries[idx] = current_physical_address | 0b10000011; //| PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
       // serial_printh("physical = ", get_p3_phys_address(p2->entries[i]));
      
    }
    if(p3->entries[ids.pdpt3] != 0 && p3->entries[ids.pdpt3] != ((uint64_t)(p2)) | 0b11){
                serial_printi("conflicting overlap in p3 @", ids.pdpt3); return; }

     p3->entries[ids.pdpt3] = ((uint64_t)(p2)) | 0b11; // | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
    //p4->entries[ids.p4] = (uint64_t)(p3) | 0b11;// | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
    
    serial_printh("ID mapped ", addr); 
   

    flush_tlb();
    invalidate_page(addr);
    
}

void init_heap()
{
    debugf("init_heap()\n");
  //  map_to_physical_new(&p2_heap, &p3_heap, p4, HEAP_PHYS, HEAP_VIRT, HEAP_SIZE);
    uintptr_t heap_phys = pmm_kalloc(HEAP_SIZE);
    ASSERT(heap_phys);

    ASSERT(map_phys_addr(HEAP_VIRT, heap_phys, HEAP_SIZE, 0));

    alloc_init();
}

void paging_init()
{
    debugf("==init paging==\n");
    //reserve framebuffer first

    pmm_mark_frames_used(sysinfo.fb.addr, 0x300000ULL); //it already should be reserved anyhow but jic
    debugf("unmapping initial identity\n");
   
    palloc_init();
                                                    //its hella problematic that we are mapping the framebuffer to qemu default address
                                                    //so at least we should ID map it instead of VIRTMAP bullcrap.. TODO
   
    debugf("mapping framebuffer %lx to %lx\n", sysinfo.fb.addr, VIRTMAP);
   ASSERT( map_phys_addr(VIRTMAP, sysinfo.fb.addr, 0x300000ULL,0)) ;


    init_heap();
   //so for user heap we just write an allocator in our libc that use brk() type syscall!
    debugf("==paging init ok==\n");

}




size_t expand_heap(void* heap_ptr, size_t size_to_add)
{
    heap_t* heap = (heap_t*)(heap_ptr);
    int num_pages = calc_num_pages(size_to_add);

    debugf("expanding heap by %lx, adding %i pages\n", size_to_add, num_pages);

    uintptr_t cr3 = get_cr3();   
    page_table_t* p4 = (page_table_t*)(cr3);
    size_t new_heap_size = heap->size + (num_pages * PAGE_SIZE);
    if(new_heap_size > HEAP_MAX_SIZE){
        return 0; 
    }

    //love when new good code coexists with old bad code
    uintptr_t heap_phys = pmm_kalloc(num_pages * PAGE_SIZE);
    ASSERT(heap_phys);

    uintptr_t heap_virtual = round_up_to_page(HEAP_VIRT + heap->size);
    debugf("heap virtual %lx, from heap addr @ %lx", heap->addr, heap_virtual);


    ASSERT(map_phys_addr(heap_virtual, heap_phys, num_pages * PAGE_SIZE, 0));

    flush_tlb();
    heap->size = new_heap_size;

    return heap->size;
}



uintptr_t virt_to_phys(uintptr_t virt) //virt addr === 56 bits
{
    //mask out offset within page
    uintptr_t ovirt = virt;
    virt &=  0xFFFFFFFFFFE00000ull;

    page_indices_t ind;
    get_page_index_vm(virt, &ind);

    //lets walk some tables

    pt_t* p4 = get_p4(); ASSERT(p4);
    uint64_t p3_ptr = (p4->entries[ind.p4]);
    pt_t* p3 = (pt_t*)(p3_ptr & 0xFFFFFFFFFFFFFF00ull);
       debugf("p3_ ptr = %lx p4 = %lx p3 = %lx \n", p3_ptr, (uint64_t)p4, (uint64_t)p3 );
    if(!p3){
        debugf("failed to translate vaddr %lx: p4 entry missing at %i \n", virt, ind.p4); return nullptr;
    }
    pt_t* p2 = (pt_t*) ( p3->entries[ind.pdpt3]  & 0xFFFFFFFFFFFFFF00ull ); 
    if(!p2){
        debugf("failed to translate vaddr %lx: p3 entry missing at %i, %lx \n", virt, ind.pdpt3, (uintptr_t)p3->entries + (8 * ind.pdpt3)); return nullptr;
    }
    uint64_t p1 = p2->entries[ind.pd2];
    if(!p1){
        debugf("failed to translate vaddr %lx: p2 entry missing at %i \n", virt, ind.pd2); return nullptr;
    }
    uintptr_t phys = p1 &  0xFFFFFFFFFFFFFF00ull;
    uintptr_t offset = ovirt - virt;
  
    return phys + offset;
}





uintptr_t map_phys_addr(uintptr_t virt, uintptr_t phys, size_t size, uint64_t flags) //this maps on kernel page table!
{
    if(flags == 0) flags = 0b10000011LLU; 
    //our virt address has gotta meet some requirements and a smart man would check those here
    ASSERT(is_page_aligned(virt)); //ill check one
   
    //save original address 
    uintptr_t o_phys = phys;
   

    //align phys if needed
    if(!is_page_aligned(phys)){
         debugf("note: page aligning requested physaddr (%lx) to %lx\n", phys, phys & BITMASK_21_ALIGN);
    
        phys = phys & BITMASK_21_ALIGN;
    }
    ASSERT(is_page_aligned(phys));


    int num_pages = calc_num_pages(size); //announce what we will do now 
    debugf("map_phys_addr():  %lx -> %lx | %i pages \n", phys, virt, num_pages);
    
    page_indices_t ind; get_page_index_vm(virt, &ind);

    //edge cases we wont handle (yet): p2 511 and > 1 page size 
    
    page_table_t* p4, *p3,*p2; p4 = p3 = p2 = 0;

    p4 = get_p4();
   
    debugf("p4 = %lx\n", (uintptr_t)p4);
    if(p4 < KERNEL_ADDR) p4 = (page_table_t*)((uint64_t)p4 + KERNEL_ADDR) ;
    debugf("p4 = %lx\n", (uintptr_t)p4);
    //okay so whats the damage: 
    //need to check tables at indices seeing what we need to allocate 

    uintptr_t p3_phys = pt_addr( p4->entries[ind.p4] );
    if(p3_phys != 0){
         debugf("p4->[p3ind]=%lx\n", p3_phys);
        p3 =  (page_table_t*)(p3_phys + KERNEL_ADDR);
    }
   
     debugf("initial p3 = %lx p4 = %lx, p3_phys = %lx  \n", (uintptr_t)p3, (uintptr_t)p4 );
    uintptr_t p2_addr = 0;
    //our logic for finding or making new tables as needed and mapping p4->p3->p2
    if(!p3){
        p3 = palloc(); //PHYS 
        p2 = palloc(); //PHYS
       
        ASSERT(p3 && p2); 
        //set p4 to p3 PHYSICAL
        
        p4->entries[ind.p4] = (uintptr_t)p3 |  (PT_FLAGS | 0b100) ; //p3 is phys since we just made it 


        debugf("made new: **p3 = %lx p2 = %lx entry = %lb \n", p3, p2,  p4->entries[ind.p4] );
        //set p2 phys since we made it
       
    }
    else {
        p2 = pt_addr( p3->entries[ind.pdpt3] ); //pt_addr removes or'ed flagsto make entry addr valid
        if(!p2){
            p2 = palloc();
            ASSERT(p2);
            //set p2 phys since we made it via malloc (returns virtual address)
            debugf("made new: p2\n");
        } 
    }
    debugf("we have: p3 = %lx p2 = %lx \n", (uintptr_t)p3, (uintptr_t)p2); //we should have a valid p3 and p2 now

     //okay so whether we made a table or found one, we set it to either the existing or new address respectively
    if((uintptr_t)p3 < KERNEL_ADDR) p3 = (page_table_t*)((uint64_t)p3 + KERNEL_ADDR); //now virt
    
     debugf("making p3 virtual: now %lx \n", (uintptr_t)p3);
    p3->entries[ind.pdpt3] = ((uintptr_t)p2 | (PT_FLAGS | 0b100)); //p2 still is physical
     
    //we now should have p4 -> p3, and p3-> p2, and either made new tables or the existing ones were used where needed 
    //p2 should be valid now and ready for our mapping 

    if((uintptr_t)p2 < KERNEL_ADDR) p2 = (page_table_t*)((uint64_t)p2 + KERNEL_ADDR); //p2 is now virtual, are you confused yet?
    debugf("making p2 virtual: now %lx \n", (uintptr_t)p3);

    for(int i = 0; i < num_pages; ++i){ //we can setup our mappings for the pages we need
        uint64_t current_virtual_address = virt + (i * PAGE_SIZE);
        uint64_t current_physical_address = phys + (i * PAGE_SIZE); //page size = 2MiB

        int idx = (current_virtual_address >> 21) & 0x1FF; //index will change if num_pages > 1 so calc on the fly
        p2->entries[idx] = current_physical_address |  flags; //add our flags and magic 
        //https://wiki.osdev.org/images/thumb/6/6b/64-bit_page_tables2.png/412px-64-bit_page_tables2.png
        debugf("entry %i @ %lx = %lx \n %lb", i,(p2->entries + 8 * idx),  p2->entries[idx], p2->entries[idx]);
    }

    //double whammy - ensure page changes went through
    flush_tlb();
    invalidate_page(virt); 
    


    // take any physical offsets we page aligned out and apply them to the virtual address

    uintptr_t offset = o_phys - phys;
    //say a prayer and return virtual address we mapped 
    return virt + offset;
    //page fault in 3, 2, 1 ...


    /*
    mapping to 0xbfe00000
            0b101111111110 0000 0000 0000 0000 0000 = fe
            0b101111111111 0000 0000 0000 0000 0000 = ff
            0b10111111111100000000000010000001 
            0xbff00081
    
    */
}

uintptr_t map_phys_addr_safe(uintptr_t virt, uintptr_t phys, size_t size, uint64_t flags) //this maps on kernel page table!
{
    if(flags == 0) flags = 0b10000011LLU; 
    //our virt address has gotta meet some requirements and a smart man would check those here
    ASSERT(is_page_aligned(virt)); //ill check one
   
    //save original address 
    uintptr_t o_phys = phys;
   

    //align phys if needed
    if(!is_page_aligned(phys)){
         debugf("note: page aligning requested physaddr (%lx) to %lx\n", phys, phys & BITMASK_21_ALIGN);
    
        phys = phys & BITMASK_21_ALIGN;
    }
    ASSERT(is_page_aligned(phys));


    int num_pages = calc_num_pages(size); //announce what we will do now 
    debugf("map_phys_addr():  %lx -> %lx | %i pages \n", phys, virt, num_pages);
    
    page_indices_t ind; get_page_index_vm(virt, &ind);

    //edge cases we wont handle (yet): p2 511 and > 1 page size 
    
    page_table_t* p4, *p3,*p2; p4 = p3 = p2 = 0;

    p4 = get_p4();
    uintptr_t phys_offset = 0;
    debugf("p4 = %lx\n", (uintptr_t)p4);
    if(p4 < KERNEL_ADDR){
        if(p4 + KERNEL_ADDR > KERN_VAS_END){
            //shit we gotta map pages for our pages
            //god damn it 


        }

        p4 = (page_table_t*)((uint64_t)p4 + phys_offset) ;
    } 
    debugf("p4 = %lx\n", (uintptr_t)p4);
    //okay so whats the damage: 
    //need to check tables at indices seeing what we need to allocate 

    uintptr_t p3_phys = pt_addr( p4->entries[ind.p4] );
    if(p3_phys != 0){
         debugf("p4->[p3ind]=%lx\n", p3_phys);
        p3 =  (page_table_t*)(p3_phys + phys_offset);
    }
   
     debugf("initial p3 = %lx p4 = %lx, p3_phys = %lx  \n", (uintptr_t)p3, (uintptr_t)p4 );
    uintptr_t p2_addr = 0;
    //our logic for finding or making new tables as needed and mapping p4->p3->p2
    if(!p3){
        p3 = palloc(); //PHYS 
        p2 = palloc(); //PHYS
       
        ASSERT(p3 && p2); 
        //set p4 to p3 PHYSICAL
        
        p4->entries[ind.p4] = (uintptr_t)p3 |  (PT_FLAGS | 0b100) ; //p3 is phys since we just made it 


        debugf("made new: **p3 = %lx p2 = %lx entry = %lb \n", p3, p2,  p4->entries[ind.p4] );
        //set p2 phys since we made it
       
    }
    else {
        p2 = pt_addr( p3->entries[ind.pdpt3] ); //pt_addr removes or'ed flagsto make entry addr valid
        if(!p2){
            p2 = palloc();
            ASSERT(p2);
            //set p2 phys since we made it via malloc (returns virtual address)
            debugf("made new: p2\n");
        } 
    }
    debugf("we have: p3 = %lx p2 = %lx \n", (uintptr_t)p3, (uintptr_t)p2); //we should have a valid p3 and p2 now

     //okay so whether we made a table or found one, we set it to either the existing or new address respectively
    if((uintptr_t)p3 < phys_offset) p3 = (page_table_t*)((uint64_t)p3 + phys_offset); //now virt
    
     debugf("making p3 virtual: now %lx \n", (uintptr_t)p3);
    p3->entries[ind.pdpt3] = ((uintptr_t)p2 | (PT_FLAGS | 0b100)); //p2 still is physical
     
    //we now should have p4 -> p3, and p3-> p2, and either made new tables or the existing ones were used where needed 
    //p2 should be valid now and ready for our mapping 

    if((uintptr_t)p2 < KERNEL_ADDR) p2 = (page_table_t*)((uint64_t)p2 + phys_offset); //p2 is now virtual, are you confused yet?
    debugf("making p2 virtual: now %lx \n", (uintptr_t)p3);

    for(int i = 0; i < num_pages; ++i){ //we can setup our mappings for the pages we need
        uint64_t current_virtual_address = virt + (i * PAGE_SIZE);
        uint64_t current_physical_address = phys + (i * PAGE_SIZE); //page size = 2MiB

        int idx = (current_virtual_address >> 21) & 0x1FF; //index will change if num_pages > 1 so calc on the fly
        p2->entries[idx] = current_physical_address |  flags; //add our flags and magic 
        //https://wiki.osdev.org/images/thumb/6/6b/64-bit_page_tables2.png/412px-64-bit_page_tables2.png
        debugf("entry %i @ %lx = %lx \n %lb", i,(p2->entries + 8 * idx),  p2->entries[idx], p2->entries[idx]);
    }

    //double whammy - ensure page changes went through
    flush_tlb();
    invalidate_page(virt); 
    


    // take any physical offsets we page aligned out and apply them to the virtual address

    uintptr_t offset = o_phys - phys;
    //say a prayer and return virtual address we mapped 
    return virt + offset;
    //page fault in 3, 2, 1 ...


    /*
    mapping to 0xbfe00000
            0b101111111110 0000 0000 0000 0000 0000 = fe
            0b101111111111 0000 0000 0000 0000 0000 = ff
            0b10111111111100000000000010000001 
            0xbff00081
    
    */
}

uintptr_t map_user_page_tables(uintptr_t virt, uintptr_t phys, size_t size, user_pt_t* pt)
{
    ASSERT(is_page_aligned(virt)); //ill check one
   
    //align phys if needed
    if(!is_page_aligned(phys)){
         debugf("note: page aligning requested physaddr (%lx) to %lx\n", phys, phys & BITMASK_21_ALIGN);
    
        phys = phys & BITMASK_21_ALIGN;
    }
    ASSERT(is_page_aligned(phys));


    int num_pages = calc_num_pages(size); //announce what we will do now 
    debugf("map_user_page_tables:  %lx -> %lx | %i pages \n", phys, virt, num_pages);
    
    page_indices_t ind; get_page_index_vm(virt, &ind);

    memset((void*)pt->p4, 0, sizeof(page_table_t));
    memset((void*)pt->p3, 0, sizeof(page_table_t));
    memset((void*)pt->p2, 0, sizeof(page_table_t));

    pt->p4->entries[ind.p4] = (uintptr_t)pt->p3p |  (PT_FLAGS | 0b100) ; //map p4 to phys addr of p3

    pt->p3->entries[ind.pdpt3] = ((uintptr_t)pt->p2p | (PT_FLAGS | 0b100)); //map p3 to phys addr of p2
     
    

    for(int i = 0; i < num_pages; ++i){ //we can setup our mappings for the pages we need
        uint64_t current_virtual_address = virt + (i * PAGE_SIZE);
        uint64_t current_physical_address = phys + (i * PAGE_SIZE); //page size = 2MiB

        int idx = (current_virtual_address >> 21) & 0x1FF; //index will change if num_pages > 1 so calc on the fly
        pt->p2->entries[idx] = current_physical_address |  (PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER); //add our flags and magic 
     
    }
    //now that we have mapped our user stuff, copy kernel mapping
    page_table_t* kp4 = get_p4(); //physical
    
   
   
    debugf("kp4 phys = %lx\n", (uintptr_t)kp4);
    if(kp4 < KERNEL_ADDR) kp4 = (page_table_t*)((uint64_t)kp4 + KERNEL_ADDR) ;
    debugf("kp4 virt = %lx\n", (uintptr_t)kp4);
    debugf("copying kp4 at idx %i (contents %lx) to user p4\n",KERNEL_P4_IDX, kp4->entries[KERNEL_P4_IDX]);
    pt->p4->entries[KERNEL_P4_IDX] =  kp4->entries[KERNEL_P4_IDX];

    page_indices_t usri; get_page_index_vm(virt, &usri);

    //we also need kernel heap stuff?
    get_page_index_vm(HEAP_VIRT, &ind);

    pt->p4->entries[ind.p4] =  kp4->entries[ind.p4];

    get_page_index_vm(sysinfo.fb.addr, &ind); //fuck it fb too
    if(usri.p4 == ind.p4){
       
        //need kp3
         uintptr_t kp3_phys = pt_addr( kp4->entries[ind.p4] );
        if(kp3_phys != 0){
            if (kp3_phys < KERNEL_ADDR) kp3_phys = kp3_phys + KERNEL_ADDR;
           page_table_t* kp3 = (page_table_t*)(kp3_phys);

           ASSERT(usri.pdpt3 != ind.pdpt3); //i swear if this ever fires 
           pt->p3->entries[ind.pdpt3] = kp3->entries[ind.pdpt3];

        }
    } else{
        pt->p4->entries[ind.p4] =  kp4->entries[ind.p4];
    }
    

    

    return virt;
}




uintptr_t unmap_phys_addr(uintptr_t virt, size_t size) //BROKEN
{
   
    //our virt address has gotta meet some requirements and a smart man would check those here
    ASSERT(is_page_aligned(virt)); //ill check one
   

   

    

    int num_pages = calc_num_pages(size); //announce what we will do now 
    debugf("unmap_phys_addr():  unmapping %lx  | %i pages \n", virt, num_pages);
    
    page_indices_t ind; get_page_index_vm(virt, &ind);

   
    
    page_table_t* p4, *p3,*p2; p4 = p3 = p2 = 0;

    p4 = get_p4();

   
    if(p4 < KERNEL_ADDR) p4 = (page_table_t*)((uint64_t)p4 + KERNEL_ADDR) ;
    debugf("p4 = %lx\n", (uintptr_t)p4);
    
    uintptr_t p3_phys = pt_addr( p4->entries[ind.p4] );
    if(p3_phys != 0){
         debugf("p4->[p3ind]=%lx\n", p3_phys);
        p3 =  (page_table_t*)(p3_phys + KERNEL_ADDR);
    } else return 1;

    

    p2 = pt_addr( p3->entries[ind.pdpt3] );
    
    
    if(!p2) return 1;
    
    debugf("we have: p3 = %lx p2 = %lx \n", (uintptr_t)p3, (uintptr_t)p2); //we should have a valid p3 and p2 now
    if((uintptr_t)p2 < KERNEL_ADDR) p2 = (page_table_t*)((uint64_t)p2 + KERNEL_ADDR); //p2 is now virtual, are you confused yet?
    debugf("making p2 virtual: now %lx \n", (uintptr_t)p3);
 
   // p3->entries[ind.pdpt3] = ((uintptr_t)p2 | PT_FLAGS);
     
 
    //so instead of being totally being wasteful here and just killing the p2 and wasting 4kb :/
    //we just map it all to 0
    for(int i = 0; i < num_pages; ++i){ 
        uint64_t current_virtual_address = virt + (i * PAGE_SIZE);
        int idx = (current_virtual_address >> 21) & 0x1FF; 
        p2->entries[idx] = 0llu;
    }

    //double whammy - ensure page changes went through
    flush_tlb();
    invalidate_page(virt); 
    
    return 0;
}