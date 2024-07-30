#include <kernel/mem/virtmem.h>
#include <kernel/mem/physmem.h>
#include <kernel/printk.h>

//from randos

extern uint64_t get_cr3();
void flush_tlb(){
     __asm__ volatile ("movq rax, cr3; movq cr3, rax; nop; movq rax, cr3; movq cr3, rax;"); //this is old but i guess i was paranoid?
    debugf("==flushed TLB==");
}

void invalidate_page(uintptr_t virtual_addr){
    __asm__ volatile ("mov rax, (%0); invlpg [rax]" : : "r"(virtual_addr)); 
    debugf("invlpg vaddr %lx", virtual_addr);
    //https://forum.osdev.org/viewtopic.php?t=24676 masm=intel bug
}



typedef struct pt_alloc_t {
    page_table_t dirs[511];

    size_t idx;
    struct pt_alloc_t* prev;
    struct pt_alloc_t* next;

    
} PACKED pt_alloc_t;

static_assert(sizeof( pt_alloc_t) == PAGE_SIZE - (4096 - 24), "pt_alloc != pagesize");

phys_addr_t alloc_pt_block(){
    phys_addr_t page = phys_malloc(PAGE_SIZE);
    assert(page);
    memset(KERNEL_VADDR + page, 0, PAGE_SIZE);
    pt_alloc_t* pta = (pt_alloc_t*)(KERNEL_VADDR + page);
    for(int i = 0; i < 512; ++i)
        memset(&pta->dirs[i].entries[0], 0, 4096);

    debugf("vmm: making new pt_alloc_block");
    return page;
}

pagedir_t* new_page_table(){
   
    static pt_alloc_t* cur = 0;

    if(cur == 0){
        cur = (pt_alloc_t*) ( alloc_pt_block() + KERNEL_VADDR);
    } //makes our first one
  
    if(cur->idx >= 511){ //end of block
        pt_alloc_t* prev = cur;
        cur = (pt_alloc_t*) ( alloc_pt_block() + KERNEL_VADDR);
        prev->next = cur;
        cur->prev = prev;
    }
     debugf("%s -> %lx", __FUNCTION__, (pagedir_t*)&(cur->dirs[cur->idx++]));
    return (pagedir_t*)&(cur->dirs[cur->idx++]);
}
static inline uintptr_t pt_addr(uintptr_t entry){ 
    return (uintptr_t)((entry & 0xFFFFFFFFFFFFFF00ull) >> 3);
}
static uint64_t *virt_get_next_map_level(pagedir_t* page_directory, uintptr_t index, uint32_t flags) {
    if(!page_directory) panic( __FUNCTION__);
  //  debugf("%s -> %lx[%li] | %b ", __FUNCTION__, page_directory, index, flags);
  
    if (page_directory[index] & 1) {
     //   debugf("    %s -> found", __FUNCTION__);
      

         uintptr_t nextlevel = (page_directory[index] & ~0x1ff ) + KERNEL_VADDR;
   //    debugf("    >nextlevel = %lx vs %lx ", nextlevel, page_directory[index]);
        return nextlevel;
    } else {
        pagedir_t* npt = new_page_table();
        //  debugf("    %s -> making new %lx", __FUNCTION__, npt);
        uint64_t new_entry = (uint64_t)((uintptr_t)npt - KERNEL_VADDR) | flags;
        page_directory[index] = new_entry;
      //  debugf("    %s -> %lx[%li] | %lx","setting new ", page_directory, index, new_entry);

   
         uintptr_t nextlevel = (uintptr_t)npt;
       // debugf("    >nextlevel = %lx", nextlevel);
        return nextlevel;
    }
}

void dump_pagetable(pagedir_t* pt)
{
    debugf("dumping pt %lx", pt);
    int e = 0;
    for(int i = 0; i < 512; ++i){
        if(pt[i]){
            debugf("PT entry @ %i", i);
            e++;
        }
    }
    debugf("total %i entries in pt", e);
}


void *virt_map_page(pagedir_t pt, phys_addr_t phys, uintptr_t virt, uint32_t flags)
{
    virt = page_align(virt);
    phys = page_align(phys);
    debugf("%s(p4 = %lx, phys = %lx, virt = %lx, flags = %u)", __FUNCTION__, pt, phys, virt, flags);


    uintptr_t i4 = (virt >> 39) & PAGEINDEXMASK;
    uintptr_t i3 = (virt >> 30) & PAGEINDEXMASK;
    uintptr_t i2 = (virt >> 21) & PAGEINDEXMASK;
    uintptr_t i1 = (virt >> 12) & PAGEINDEXMASK;

   // debugf(" i4 = %li i3 = %li i2 = %li i1 = %li", i4, i3, i2, i1);
    uint64_t* p4 = pt;
    uint64_t* p3 = 0;
    uint64_t* p2 = 0;
   
    p3 = virt_get_next_map_level(p4, i4, PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG);

  
    p2 = virt_get_next_map_level(p3, i3, PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG);
  
  //  debugf(" p3 = %lx p2 = %lx", p3, p2);
    p2[i2] = phys | flags;

   
    flush_tlb();
     invalidate_page(virt);
 //   dump_pagetable(p3);
   // dump_pagetable(p2);
    return virt;
}

void virt_unmap_page(pagedir_t pt, uintptr_t virt)
{
    virt_map_page(pt, 0, virt, 0);
}





extern uintptr_t p2_table;
extern uintptr_t p3_table;
extern uintptr_t p4_table;

pagedir_t* virt_current_pd(){
    return get_cr3() + KERNEL_VADDR;
}

void virtmem_init() 
{
    debugf("virtmem_init()");
    phys_addr_t addr = phys_malloc(PAGE_SIZE * 64); //get us tf away from any random code or whatever
     debugf("virtmem_init() %lx %lx", addr, addr + KERNEL_VADDR);
    //ll
    pagedir_t *pt = get_cr3() + KERNEL_VADDR;

    dump_pagetable(pt);

    virt_map_page(pt, kboot.fb.addr, kboot.fb.addr, PAGE_SIZE_2MB | PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG);
    virt_map_page(pt, kboot.fb.addr + PAGE_SIZE, kboot.fb.addr + PAGE_SIZE, PAGE_SIZE_2MB |  PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG );
    virt_map_page(pt, kboot.fb.addr + (2 * PAGE_SIZE), kboot.fb.addr + (2 * PAGE_SIZE), PAGE_SIZE_2MB |  PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG  );
    virt_map_page(pt, kboot.fb.addr + (3 * PAGE_SIZE), kboot.fb.addr+ (3 * PAGE_SIZE), PAGE_SIZE_2MB |  PAGE_PRESENT_FLAG | PAGE_WRITE_FLAG  );
    kboot.fb.ptr = (void*)kboot.fb.addr;


}