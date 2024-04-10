#include "page.h"

#include "../../drivers/serial/serial.h"
extern uint64_t get_cr3();

extern uintptr_t p2_table;
extern uintptr_t p3_table;
extern uintptr_t p4_table;

page_table_t p2_vram;
//page_table_t p3_vram;

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
    serial_printh("idx for VA ", virtual_addr);
    idx->p4 = (virtual_addr >> 39) & 0x1FF;
    serial_printi("p4 idx ", idx->p4);
    idx->pdpt3 = (virtual_addr >> 30) & 0x1FF;
     serial_printi("p3 idx ", idx->pdpt3);
    idx->pd2 = (virtual_addr >> 21) & 0x1FF;
    serial_printi("p2 idx ", idx->pd2);
    idx->pt = (virtual_addr >> 12) & 0x1FF;
    serial_printi("p1 idx ", idx->pt);

}

uint64_t get_p3_phys_address(uint64_t p3e){
    const static uint64_t physical_address_mask = 0x000FFFFFFFFFF000; // Mask to extract bits 0-51
    return (p3e & physical_address_mask);
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
       // serial_printh("physical = ", get_p3_phys_address(p2->entries[i]));
      
    }

    //check for existing entries where we want: 
    if(p3->entries[ids.pdpt3] != 0){
                serial_printi("overlap in p3 @", ids.pdpt3); return; }
  //  if(p4->entries[ids.p4] != 0) {
   //     serial_printi("overlap in p4 @", ids.p4); return; }

    serial_printh("flags are ", PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG);
    p3->entries[ids.pdpt3] = ((uint64_t)(p2)) | 0b11; // | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
  //  p4->entries[ids.p4] = (uint64_t)(p3) | 0x23;// | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG | PSE_HUGE_FLAG;
    
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

void make_page_struct()
{
    memset(&p2_vram, 0, 512 * 8);
 //   memset(&p3_vram, 0, 512 * 8);
   // dump_pt(&p3_table, "p3=", 1);
   // dump_pt(&p2_table, "p2=", 1);
    uintptr_t cr3 = get_cr3();
    serial_printh("cr3 = ", cr3);

    page_table_t* p4 = (page_table_t*)(cr3);
   
    serial_printh("address p4 = ", (uintptr_t)p4);
    serial_printh("address p4real = ", (uintptr_t)&p4_table);

    page_table_t* p3 =  (page_table_t*) (&p3_table);
    serial_printh("address p3 = ", (uintptr_t)p3);
    serial_printh("address p3 via p4[0] = ", (uintptr_t)p4->entries[0]);
    //// serial_printh("address p3VRAM = ", (uintptr_t)&p3_vram);
    // serial_printh("address p3 real = ", (uintptr_t)p3_table);

    page_table_t* p2 =  (page_table_t*) &p2_table;
    serial_printh("address p2 = ", (uintptr_t)p2);

    uint64_t virtual_address = 0x0;
    uint64_t physical_address = 0x100000; //same as linker
   
    serial_printh("first entry p3 = ", p3->entries[0]);
   // dump_pt(p4, "p4 = ", True);
    //
 
    map_to_physical(&p2_vram, p3, p4, (uint64_t)0xfc000000ULL, (uint64_t)VIRTMAP,  0x300000ULL);
    
    serial_printh("address p2_vram = ", (uintptr_t)&p2_vram);
    dump_pt(p3, "p3", 1);
    dump_pt(p4, "p4 ", True);
    dump_pt(p2, "p2", 1);   
    dump_pt(&p2_vram, "p2 vram", 1);   


   // dump_pt(p3);

}
