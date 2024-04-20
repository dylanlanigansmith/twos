#include "pmm.h"
#include "../sys/sysinfo.h"
#include "../boot/multiboot2.h"
#include "../mem/heap.h"
#include "../mem/page.h"

#include "phytable.h"

typedef struct 
{
    size_t total_memory;

    phy_table tbl;


    
} pmm_t;

pmm_t pmm;


size_t sum_available_memory(multiboot_memory_map_t* maps, uint32_t size, bool log)
{
    size_t sum = 0;
    for(int i =0; i < size; ++i){
        if(maps[i].type == MULTIBOOT_MEMORY_AVAILABLE){

            if(log) debugf("Available Memory Region [%i]:{ @%lx - %lx } size %lx [%li MiB] \n", i, maps[i].addr, maps[i].addr + maps[i].len, maps[i].len, BYTES_TO_MIB(maps[i].len));

            sum += maps[i].len;
        }
    }
     if(log) debugf("Total Memory:\n    %li KiB  /  %li MiB  /  %li GiB\n Total Pages: %li \n\n", BYTES_TO_KIB(sum), BYTES_TO_MIB(sum), BYTES_TO_GIB(sum), (uint64_t)(sum / PAGE_SIZE)) ;
    return sum;
}

bool is_frame_free(uintptr_t addr)
{
   // 0xbffe0000
    phy_idx ids = get_indices_for_phyaddr(addr);
  //  debugf("iff: %lb",pmm.tbl.entry[ids.p4].entry[ids.p3].entry[ids.idx] );
    return get_bit_at(&pmm.tbl.entry[ids.p4].entry[ids.p3], &ids);

}

void mark_frame(uintptr_t aligned, uint8_t m)
{
    ASSERT(is_page_aligned(aligned));
    phy_idx ids = get_indices_for_phyaddr(aligned);

    if(m){
        set_bit_at(&pmm.tbl.entry[ids.p4].entry[ids.p3], &ids);
        return;
    }
    clear_bit_at(&pmm.tbl.entry[ids.p4].entry[ids.p3], &ids);
}


void pmm_init()
{
    ASSERT(sysinfo.mem.mb_map);
    debugf("\n==PMM Init==\n");

    memset(&pmm, 0, sizeof(pmm));

    struct multiboot_tag_mmap* mmaps_tag = sysinfo.mem.mb_map;
    multiboot_memory_map_t* maps = mmaps_tag->entries;  
    pmm.total_memory = sum_available_memory(maps, sysinfo.mem.mb_size, 1);

    size_t total_pages = pmm.total_memory / PAGE_SIZE;
   
    
    //    this is gonna be so fuckin slow!
    //sorry
    //at least its obivous on how to make it faster i just wanna get shit workin first
    for (int i =0; i < sysinfo.mem.mb_size; ++i) {

        if(maps[i].type != MULTIBOOT_MEMORY_AVAILABLE) continue;
        
        size_t size = maps[i].len;
        uintptr_t start = maps[i].addr;
        uintptr_t end = start + size;
        size_t num_pages = size / PAGE_SIZE;
        size_t remainder = size % PAGE_SIZE; //bummer, whatcha get for using 2mb pages
        debugf("for region %lx - %lx there are %li pages (r %li kb) to set free\n", start, end, num_pages, BYTES_TO_KIB(remainder));
        //check alignment etc 
        if(start < PAGE_SIZE) start = PAGE_SIZE;
        if(end < PAGE_SIZE) continue;

        
        
        if(!is_page_aligned(end)){
            num_pages -= 1;
            end = start + ( (num_pages )  * PAGE_SIZE);
            
            debugf("aligned start %lx - aligned end %lx (%i %i) \n", start, end, is_page_aligned(start), is_page_aligned(end));
        }
        
        
        //mark all free pages 
        for(size_t i = 0; i < num_pages; ++i){
            uintptr_t addr = start + (i * PAGE_SIZE);
            ASSERT((addr + PAGE_SIZE - 1) < end); //fail pls
            phy_idx ids = get_indices_for_phyaddr(addr);

           // pmm.tbl.entry[ids.p4].entry[ids.p3].entry[ids.idx] //this is fucked man

           set_bit_at(&pmm.tbl.entry[ids.p4].entry[ids.p3], &ids); //so like obviously we can skip ahead and set entire 64 bits at a time a la memcpy / memset 

        } 
    }

    //okay so we have marked the general free space
    //things that also take space:
        //the fucking kernel

    debugf("marking from kernel phys start %lx to kernel phys end %lx as used", sysinfo.boot.kernel_base, sysinfo.boot.kernel_end);

    ASSERT(sysinfo.boot.kernel_end > sysinfo.boot.kernel_base); 
    pmm_mark_frames_used(sysinfo.boot.kernel_base, sysinfo.boot.kernel_end - (sysinfo.boot.kernel_base));
    pmm_mark_frames_used(0, sysinfo.boot.kernel_end + PAGE_SIZE * 6);
    // so we gotta make sure that physmem goes thru here from now on and all should be well!
    // except for the cases we dont want it to lol (acpi)

    uintptr_t test = 0xbffe0000; //0xbffe0001
    //debugf("is %lx free? %i",test , is_frame_free(test));

    test = 0xbffe0001; //0xbffe0001
  //  debugf("is %lx free? %i",test , is_frame_free(test));

    
    debugf("==PMM End==\n");
}

void pmm_debug_space(){
    debugf("==PMM USAGE==\n");
    for(int i = 0; i < (BYTES_TO_GIB(pmm.total_memory) + 2) ; ++i)
    {
        uintptr_t start_addr = (uintptr_t)i * ONE_GIB;
        phy2_t* p2 = &pmm.tbl.entry[0].entry[i];
        size_t free = 0;
        for (int j = 0; j < 8; ++j){
            free += count_set_bits(p2->entry[j]);

            debugf("%s %lx: p3[%i] p2[%i]: %lb \n",(start_addr == 0 && j == 0) ?"       " : "", start_addr + (64 * j * PAGE_SIZE), i,j, p2->entry[j]);
        }
        if(!free) continue;

        
        debugf("from %lx to %lx there are %li free pages\n", start_addr, start_addr + (512 * PAGE_SIZE), free);
    }

    debugf("========\n");
}

uintptr_t pmm_kalloc(size_t size)
{

    //now that we are higher half this is outdated!!!!

    

    //finds some memory for us in 0-1gb kernel area
    size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE; //ensure round up
    uint64_t addr = 0;
    uint64_t found = 0;

    //im gonna write some really bad code now
    //sorry
imsorry:
    if(addr > 2 * ONE_GIB) return 0;
    while(!is_frame_free(addr)) addr += PAGE_SIZE;
    found = addr;
    for(int i = 0; i < num_pages; ++i){
        addr += PAGE_SIZE;
        if(!is_frame_free(addr)) goto imsorry;
    }

    debugf("pmm_kalloc(%lx): found space for %li pages at %lx\n", size, num_pages, found);

    //mark pages claimed!
    uintptr_t frame_start = found;
    for(int i = 0; i < num_pages; ++i){
        frame_start += PAGE_SIZE;
        mark_frame(frame_start, 0);
    }
    return found;
}

uintptr_t pmm_alloc(size_t size) //the bad code here is one thing, copy and pasting it is even worse lol
{

    //finds some memory for us ideally above 1gb
    size_t num_pages = (size + PAGE_SIZE - 1) / PAGE_SIZE; //ensure round up
    uint64_t addr = USER_PHYS_START;
    uint64_t found = 0;

    //im gonna write some really bad code now
    //sorry
imsorryagain:
    if(addr > pmm.total_memory) return 0; //god help us
    while(!is_frame_free(addr)) addr += PAGE_SIZE;
    found = addr;
    for(int i = 0; i < num_pages; ++i){
        addr += PAGE_SIZE;
        if(!is_frame_free(addr)) goto imsorryagain;
    }

    debugf("pmm_alloc(%lx): found space for %li pages at %lx\n", size, num_pages, found);

    //mark pages claimed!
    uintptr_t frame_start = found;
    for(int i = 0; i < num_pages; ++i){
        frame_start += PAGE_SIZE;
        mark_frame(frame_start, 0);
    }
    return found;
}


uintptr_t pmm_mark_frames_used(uintptr_t addr, size_t size)
{
    size_t o_size = size;
    if(size < PAGE_SIZE) size = PAGE_SIZE;
    if(size % PAGE_SIZE > 0) size += size % PAGE_SIZE;

    size_t num_pages = size / PAGE_SIZE;
    num_pages = (num_pages > 0) ?  num_pages : 1;
    
    uintptr_t aldr = page_align(addr);

    debugf("mark region used: %lx - %lx", addr, addr + o_size );

    debugf("marking from %lx - %lx as used, total pages = %li", aldr, aldr + size, num_pages);

    ASSERT(aldr + size > aldr );

    if( page_align(addr + o_size) > aldr + size){
        debugf("BUG HEY LOOK HERE IM AN ISSUE! PMM_MARK_FRAMES_USED line: %i\n", __LINE__);
    }

    for(int i = 0; i < num_pages; ++i)
        mark_frame(aldr + (i * PAGE_SIZE), 0);

    return num_pages;
}
