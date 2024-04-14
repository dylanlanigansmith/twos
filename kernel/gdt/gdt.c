#include "gdt.h"
#include "../stdlib.h"
typedef struct tss_entry {
	uint32_t reserved_0;
	uint64_t rsp[3];
	uint64_t reserved_1;
	uint64_t ist[7];
	uint64_t reserved_2;
	uint16_t reserved_3;
	uint16_t iomap_base;
} __attribute__ ((packed)) __attribute__((aligned(0x10))) tss_entry_t;


typedef struct {
    uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
	uint8_t granularity;
	uint8_t base_high;
	uint32_t base_highest;
	uint32_t reserved0;
} __attribute__((packed)) gdt_entry_sysseg_t;


/*
GDT_t gdt __attribute__((used)) =  {{
	{
		{0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00},
		{0xFFFF, 0x0000, 0x00, 0x9A, (1 << 5) | (1 << 7) | 0x0F, 0x00},
		{0xFFFF, 0x0000, 0x00, 0x92, (1 << 5) | (1 << 7) | 0x0F, 0x00},
		{0xFFFF, 0x0000, 0x00, 0xFA, (1 << 5) | (1 << 7) | 0x0F, 0x00},
		{0xFFFF, 0x0000, 0x00, 0xF2, (1 << 5) | (1 << 7) | 0x0F, 0x00},
		{0xFFFF, 0x0000, 0x00, 0xFA, (1 << 5) | (1 << 7) | 0x0F, 0x00},
		{0x0067, 0x0000, 0x00, 0xE9, 0x00, 0x00},
	},
	{0x00000000, 0x00000000},
	{0x0000, 0x0000000000000000},
	{0,{0,0,0},0,{0,0,0,0,0,0,0},0,0,0},
}};*/
extern uintptr_t* kernel_stack;

tss_entry_t task_state_segment ={0,{&kernel_stack,&kernel_stack,&kernel_stack},0,{0,0,0,0,0,0,0},0,0,sizeof(task_state_segment)};

extern uintptr_t  GDT_TSS_PTR;
extern void load_tss();
void make_tss(){

    
    //debugf("stack top = %lx", &kernel_stack); //wack

    //like this didnt crash and we can see it in bochs but


    //i sorta wanted it to crash so we know it did something..
    uintptr_t addr = (uintptr_t)&task_state_segment;

    gdt_entry_sysseg_t tss = {0x0067, 0x0000, 0x00, 0xE9, 0x00, 0x00, 0x00000000, 0x00000000}; 
    

    tss.limit_low = sizeof(task_state_segment);
    tss.base_low = (addr & 0xffff);
    tss.base_middle = (addr >> 16) & 0xff;
    tss.base_high = (addr >> 24) & 0xff;
    tss.base_highest = (addr >> 32) & 0xFFFFFFFF; 
    
    //fuck it stick your user mode gdt segs in too here and see if they show up!
    memcpy((void*)GDT_TSS_PTR, &tss, sizeof(tss));
 //  *(gdt_entry_sysseg_t*)(GDT_TSS_PTR) =


   load_tss();
}