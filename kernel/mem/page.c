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

    serial_printh("Extended VA: ", virtual_addr);
    debugf("idx for VA: %lx [%lb] ", virtual_addr, virtual_addr);
    idx->p4 = (virtual_addr >> 39) & 0x1FF;
    
    serial_printi("p4 idx ", idx->p4);
    idx->pdpt3 = (virtual_addr >> 30) & 0x1FF;
     serial_printi("p3 idx ", idx->pdpt3);
    idx->pd2 = (virtual_addr >> 21) & 0x1FF;
    serial_printi("p2 idx ", idx->pd2);
    idx->pt = (virtual_addr >> 12) & 0x1FF;
    serial_printi("p1 idx ", idx->pt);
    debugf(" p4 =%lu  p3 = %lu p2 = %lu p1 = %lu \n", idx->p4, idx->pdpt3, idx->pd2, idx->pt);

     debugf(" p4 =%lu  p3 = %lu p2 = %lu p1 = %lu \n", (uint64_t) (virtual_addr >> 39) & 0x1FFULL, idx->pdpt3, idx->pd2, idx->pt);

}


int calc_num_pages(uint64_t addr_size){
    return (addr_size + PAGE_SIZE - 1) / PAGE_SIZE;
}

#define P2_SHIFT 21
void map_to_physical(page_table_t* p2, page_table_t* p3, page_table_t* p4, uint64_t physical_to_map, uint64_t virtual_to_map, uint64_t size_to_map)
{
    
    int num_pages = calc_num_pages(size_to_map);
    page_indices_t ids = {0,0,0,0};
    get_page_index_vm(virtual_to_map, &ids);
    serial_printh("mapping virtual ", virtual_to_map);
    serial_printh("mapping physical ", physical_to_map);
    serial_printi("pages = ", num_pages);
    for(uint64_t i = 0; i < num_pages; ++i){
        
        uint64_t current_virtual_address = virtual_to_map + (i * PAGE_SIZE);
        uint64_t current_physical_address = physical_to_map + (i * PAGE_SIZE);
        
        int idx = (current_virtual_address >> P2_SHIFT) & 0x1ff;
        p2->entries[idx] = current_physical_address | 0b10000011; //| PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
      
    }

    //check for existing entries where we want: 
    if(p3->entries[ids.pdpt3] != 0){
                serial_printi("overlap in p3 @", ids.pdpt3); return; }

    p3->entries[ids.pdpt3] = ((uint64_t)(p2)) | 0b11; // | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
  //  p4->entries[ids.p4] = (uint64_t)(p3) | 0x23;// | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
    
    serial_printh("mapped ", virtual_to_map); 
    serial_printh("to physical addresss", physical_to_map); 

    flush_tlb();
    invalidate_page(virtual_to_map);
   // flush_tlb();
}

#define PT_FLAGS 0b11llu

void map_to_physical_new(page_table_t* p2, page_table_t* p3, page_table_t* p4, uint64_t physical_to_map, uint64_t virtual_to_map, uint64_t size_to_map)
{
    
    int num_pages = calc_num_pages(size_to_map);
    page_indices_t ids = {0,0,0,0};
    get_page_index_vm(virtual_to_map, &ids);
    serial_printh("NEW mapping virtual ", virtual_to_map);
    serial_printh("mapping physical ", physical_to_map);
    serial_printi("pages = ", num_pages);
    for(uint64_t i = 0; i < num_pages; ++i){
        
        uint64_t current_virtual_address = virtual_to_map + (i * PAGE_SIZE);
        uint64_t current_physical_address = physical_to_map + (i * PAGE_SIZE);
        
        int idx = (current_virtual_address >> P2_SHIFT) & 0x1ff;
        p2->entries[idx] = current_physical_address | 0b10000011; //| PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
       // serial_printh("physical = ", get_p3_phys_address(p2->entries[i]));
      
    }

    //check for existing entries where we want: 
    if(p3->entries[ids.pdpt3] != 0 && p3->entries[ids.pdpt3] != ((uint64_t)(p2)) | 0b11){
                serial_printi("conflicting overlap in p3 @", ids.pdpt3); return; }
   // if(p4->entries[ids.p4] != 0) {
   //     serial_printi("overlap in p4 @", ids.p4); return; }

    debugf("map_new: p3 = %lx", (uint64_t)p3);
   
    p3->entries[ids.pdpt3] = ((uint64_t)(p2)) | 0b11; // | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
    p4->entries[ids.p4] = (uint64_t)(p3) | 0b11;// | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
    
    serial_printh("mapped ", virtual_to_map); 
    serial_printh("to physical addresss", physical_to_map); 

    flush_tlb();
    invalidate_page(virtual_to_map);
   // flush_tlb();
}


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
    uintptr_t cr3 = get_cr3();
   // page_table_t* p4 = (page_table_t*)(cr3 + KERNEL_ADDR) ;  //phys
   // p4->entries[0] = 0x00000000000;
   // invalidate_page(0x00000);


    palloc_init();
    
    
    

   
    

   //page_table_t* p2_vram = palloc();
   //if(!p2_vram) KPANIC("palloc failure: framebuffer map");
                                                    //its hella problematic that we are mapping the framebuffer to qemu default address
                                                    //so at least we should ID map it instead of VIRTMAP bullcrap.. TODO
   
    debugf("mapping framebuffer %lx to %lx\n", sysinfo.fb.addr, VIRTMAP);
   ASSERT( map_phys_addr(VIRTMAP, sysinfo.fb.addr, 0x300000ULL,0)) ;

  // map_to_physical(p2_vram, p3, p4, (uint64_t)sysinfo.fb.addr, (uint64_t)VIRTMAP,  ); 
   //also trash this function please ^^^^^^^
    
    init_heap();
   //so for user heap we just write an allocator in our libc !

   //user_palloc_init();

   // page_indices_t ind;
   // get_page_index_vm(0xffffff8000000000, &ind);

    debugf("==paging init ok==\n");
  // ; __asm__ volatile ("int 3");
   //; for(;;) __asm__ volatile (" hlt");
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





uintptr_t map_phys_addr(uintptr_t virt, uintptr_t phys, size_t size, uint64_t flags)
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

uintptr_t unmap_phys_addr(uintptr_t virt, size_t size)
{
  
    //our virt address has gotta meet some requirements and a smart man would check those here
    ASSERT(is_page_aligned(virt)); //ill check one
   

   

    

    int num_pages = calc_num_pages(size); //announce what we will do now 
    debugf("unmap_phys_addr():  unmapping %lx  | %i pages \n", virt, num_pages);
    
    page_indices_t ind; get_page_index_vm(virt, &ind);

   
    
    page_table_t* p4, *p3,*p2; p4 = p3 = p2 = 0;

    p4 = get_p4();
    pt_t* p4r = get_p4(); //oh shit 
  
    p3 = pt_addr( p4->entries[ind.p4] );
    p2 = pt_addr( p3->entries[ind.pdpt3] );
    
    if(!p3) return 1;
    if(!p2) return 1;
    
    debugf("we have: p3 = %lx p2 = %lx \n", (uintptr_t)p3, (uintptr_t)p2); //we should have a valid p3 and p2 now

 
    p3->entries[ind.pdpt3] = ((uintptr_t)p2 | PT_FLAGS);
     
 

    for(int i = 0; i < num_pages; ++i){ 
        uint64_t current_virtual_address = virt + (i * PAGE_SIZE);
        int idx = (current_virtual_address >> 21) & 0x1FF; 
        p2->entries[idx] = 0;
    }

    //double whammy - ensure page changes went through
    flush_tlb();
    invalidate_page(virt); 
    
    return 0;
}

uintptr_t map_user_page_tables(uintptr_t virt, uintptr_t phys, size_t size, page_table_t *p4, page_table_t *p3, page_table_t *p2)
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

    
    p3 = pt_addr( p4->entries[ind.p4] );
     p4->entries[ind.p4] = (uintptr_t)p3 |  PT_FLAGS; //>:(

    p3->entries[ind.pdpt3] = ((uintptr_t)p2 | PT_FLAGS);
     
    

    for(int i = 0; i < num_pages; ++i){ //we can setup our mappings for the pages we need
        uint64_t current_virtual_address = virt + (i * PAGE_SIZE);
        uint64_t current_physical_address = phys + (i * PAGE_SIZE); //page size = 2MiB

        int idx = (current_virtual_address >> 21) & 0x1FF; //index will change if num_pages > 1 so calc on the fly
        p2->entries[idx] = current_physical_address |  (PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER); //add our flags and magic 
        //https://wiki.osdev.org/images/thumb/6/6b/64-bit_page_tables2.png/412px-64-bit_page_tables2.png
       // debugf("entry %i @ %lx = %lx \n %lb", i,(p2->entries + 8 * idx),  p2->entries[idx], p2->entries[idx]);
    }

    return virt;
}

/*
solutions
add page align to malloc

requires refactor so that header/footers only made for empty spots (cant align header)

or
identity map a
page heap
page heap
page heap
page heap
PAGE HEAP PAGE HEAP PAGE HEAP PAGE HEAP PAGE HEAP PAGE HEAP PAGE HEAP PAGE HEAP PAGE HEAP
find a block of mem and map it greasy way like we do for heap/fb

write freeless allocator

OR

do recursive

*/