#include "gdt.h"
#include "../stdlib.h"
typedef struct {
	uint32_t reserved_0;
	uint64_t rsp[3];
	uint64_t reserved_1;
	uint64_t ist[7];
	uint64_t reserved_2;
	uint16_t reserved_3;
	uint16_t iomap_base;
} __attribute__ ( (packed, aligned(0x8)) ) tss_t; //ideally we would get 16 bit aligned or better but beggars cant be chosers
												//if this is a problem we can just prealloq aligned space in .rodata
static_assert(sizeof(tss_t) == 104, "tss size error");

/*
;intel manual pg. 3098
access byte
	 ;7 = present 
    ;5/6 = 0-3 priv level (3 = user)
    ;4 if not set segment is system
    ;3 = executable
    ; 2 DC - for code if 1 can be exec from equal or lower priv
	 ;1 Readable/Writable - code = read, data = write
    ; 0 = accessed, set to 1 so cpu doesnt waste time doing so or if r/o

*/

typedef struct {
	uint8_t accessed : 1; //just set it for the cpu ahead of time
	uint8_t RW : 1; //high if read for code, write for data
	uint8_t DC : 1; //high for code if can be exec from eql or lower priv
	uint8_t exec : 1; 
	uint8_t not_sys : 1; //high if segment isnt a system one (YES)
	uint8_t priv : 2; //0 -3
	uint8_t present : 1;
} __attribute__((packed)) access_bit_t;
static_assert(sizeof(access_bit_t) == 1, "gdt access bitfields bad");


//https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
typedef struct {
    uint16_t size; //limit is size 
	uint16_t base_0_15;
	uint8_t base_16_23;
	access_bit_t access;
	uint8_t flags; 
	uint8_t base_24_31;
	uint32_t base_31_64;
	uint32_t reserved0;
} __attribute__((packed)) gdt_entry_sys_t; 
//good source for TSS Descriptor
//https://xem.github.io/minix86/manual/intel-x86-and-64-manual-vol3/o_fe12b1e2a880e0ce245.png


typedef struct {
	uint16_t limit_0_15;
	uint16_t base_0_15;
	uint8_t base_16_23;
	access_bit_t access;
	uint8_t limit_16_19_and_flags; 
	uint8_t base_24_31;
} __attribute__((packed)) gdt_entry_t;

static_assert(sizeof(gdt_entry_t) == 8, "gdt_entry_t wrong size"); 
static_assert(sizeof(gdt_entry_sys_t) == 16, "gdt_entry_sys_t wrong size"); 



typedef struct {
	gdt_entry_t null;
	gdt_entry_t kernel_cs;
	gdt_entry_t kernel_ds;
	gdt_entry_t user_cs;
	gdt_entry_t user_ds;
	gdt_entry_sys_t tss;
} __attribute__((packed)) GDT; 

static_assert(sizeof(GDT) == (sizeof(gdt_entry_t) * GDT_ENTRIES  + sizeof(gdt_entry_sys_t)), "GDT_real struct issues" );


typedef struct { 
	uint16_t limit;
	uintptr_t base;
} __attribute__((packed)) gdtr_t;
//ours is at GDT_PTR + sizeof(gdt) 

extern uintptr_t kernel_stack;
extern uintptr_t kernel_stack_end;


extern uintptr_t  GDT_TSS_PTR;
extern uintptr_t  GDT_PTR;



tss_t task_state_segment = {0,{0,0,0},0,{0,0,0,0,0,0,0},0,0,0};

/*
So we make a "new GDT" right where our old one was 
*/

static void dump_gdt(GDT* gdt, bool dump_sys)
{
	//(see below for) the only good use of chatgpt 
	 debug("===DUMP GDT===");
    

	debugf("gdt->kernel_cs:\n");
    debugf("  limit_0_15: %x\n", gdt->kernel_cs.limit_0_15);
    debugf("  base_0_15: %x\n", gdt->kernel_cs.base_0_15);
    debugf("  base_16_23: %x\n", gdt->kernel_cs.base_16_23);
	 debug("  ---access bit--- \n");
    debugf("  	accessed: %x\n", gdt->kernel_cs.access.accessed);
    debugf("  	RW: %x\n", gdt->kernel_cs.access.RW);
    debugf("  	DC: %x\n", gdt->kernel_cs.access.DC);
    debugf("  	exec: %x\n", gdt->kernel_cs.access.exec);
    debugf("  	not_sys: %x\n", gdt->kernel_cs.access.not_sys);
    debugf("  	priv: %x\n", gdt->kernel_cs.access.priv);
    debugf("  	present: %x\n", gdt->kernel_cs.access.present);
    debugf("  limit_16_19_and_flags: %x %B\n", gdt->kernel_cs.limit_16_19_and_flags, gdt->kernel_cs.limit_16_19_and_flags);
    debugf("  base_24_31: %x\n", gdt->kernel_cs.base_24_31);

    debugf("gdt->kernel_ds:\n");
    debugf("  limit_0_15: %x\n", gdt->kernel_ds.limit_0_15);
    debugf("  base_0_15: %x\n", gdt->kernel_ds.base_0_15);
    debugf("  base_16_23: %x\n", gdt->kernel_ds.base_16_23);
	 debug("  ---access bit--- \n");
    debugf("    accessed: %x\n", gdt->kernel_ds.access.accessed);
    debugf("  	RW: %x\n", gdt->kernel_ds.access.RW);
    debugf("    DC: %x\n", gdt->kernel_ds.access.DC);
    debugf("    exec: %x\n", gdt->kernel_ds.access.exec);
    debugf("    not_sys: %x\n", gdt->kernel_ds.access.not_sys);
    debugf("    priv: %x\n", gdt->kernel_ds.access.priv);
    debugf("    present: %x\n", gdt->kernel_ds.access.present);
    debugf("  limit_16_19_and_flags: %x %B\n", gdt->kernel_ds.limit_16_19_and_flags, gdt->kernel_ds.limit_16_19_and_flags);
    debugf("  base_24_31: %x\n", gdt->kernel_ds.base_24_31);
    
    debugf("gdt->user_cs:\n");
    debugf("  limit_0_15: %x\n", gdt->user_cs.limit_0_15);
    debugf("  base_0_15: %x\n", gdt->user_cs.base_0_15);
    debugf("  base_16_23: %x\n", gdt->user_cs.base_16_23);
	 debug("  ---access bit--- \n");
    debugf("  	accessed: %x\n", gdt->user_cs.access.accessed);
    debugf("  	RW: %x\n", gdt->user_cs.access.RW);
    debugf("  	DC: %x\n", gdt->user_cs.access.DC);
    debugf("  	exec: %x\n", gdt->user_cs.access.exec);
    debugf("  	not_sys: %x\n", gdt->user_cs.access.not_sys);
    debugf("  	priv: %x\n", gdt->user_cs.access.priv);
    debugf("  	present: %x\n", gdt->user_cs.access.present);
    debugf("  limit_16_19_and_flags: %x %B\n", gdt->user_cs.limit_16_19_and_flags, gdt->user_cs.limit_16_19_and_flags);
    debugf("  base_24_31: %x\n", gdt->user_cs.base_24_31);

    
    debugf("gdt->user_ds:\n");
    debugf("  limit_0_15: %x\n", gdt->user_ds.limit_0_15);
    debugf("  base_0_15: %x\n", gdt->user_ds.base_0_15);
    debugf("  base_16_23: %x\n", gdt->user_ds.base_16_23);
	 debug("  ---access bit--- \n");
    debugf("  	accessed: %x\n", gdt->user_ds.access.accessed);
    debugf("  	RW: %x\n", gdt->user_ds.access.RW);
    debugf("  	DC: %x\n", gdt->user_ds.access.DC);
    debugf("  	exec: %x\n", gdt->user_ds.access.exec);
    debugf("  	not_sys: %x\n", gdt->user_ds.access.not_sys);
    debugf("  	priv: %x\n", gdt->user_ds.access.priv);
    debugf("  	present: %x\n", gdt->user_ds.access.present);
    debugf("  limit_16_19_and_flags: %x %B\n", gdt->user_ds.limit_16_19_and_flags, gdt->user_ds.limit_16_19_and_flags);
    debugf("  base_24_31: %x\n", gdt->user_ds.base_24_31);

	if(!dump_sys){
		debug("not dumping gdt->tss+ \n");
		debug("=== END GDT DUMP === \n \n");
		return;
	}

	debugf("gdt->tss:\n");
    debugf("  size: %x\n", gdt->tss.size);
    debugf("  base_0_15: %x\n", gdt->tss.base_0_15);
    debugf("  base_16_23: %x\n", gdt->tss.base_16_23);
	 debug("  ---access bit--- \n");
    debugf("  	accessed: %x\n", gdt->tss.access.accessed);
    debugf("  	RW: %x\n", gdt->tss.access.RW);
    debugf("  	DC: %x\n", gdt->tss.access.DC);
    debugf(" 	exec: %x\n", gdt->tss.access.exec);
    debugf("  	not_sys: %x\n", gdt->tss.access.not_sys);
    debugf("  	priv: %x\n", gdt->tss.access.priv);
    debugf(" 	present: %x\n", gdt->tss.access.present);
    debugf("  flags: %x\n", gdt->tss.flags);
    debugf("  base_24_31: %x\n", gdt->tss.base_24_31);
    debugf("  base_31_64: %x\n", gdt->tss.base_31_64);
    debugf("  reserved0: %x\n", gdt->tss.reserved0);

	 debug("=== END GDT DUMP === \n \n");
}

void init_gdt(GDT* gdt)
{

	debugf("gdt ptr = %lx, tss ptr = %lx tss - gdt = %lx", GDT_PTR, GDT_TSS_PTR, GDT_TSS_PTR - GDT_PTR); //wack

	
	ASSERT( (GDT_TSS_PTR - GDT_PTR)  == TSS_GDT_OFFSET );

	

}
void make_tss()
{

    debugf("stack top = %lx end = %lx size = %lx", kernel_stack, kernel_stack_end, kernel_stack - kernel_stack_end); //wack
	
	memset(&task_state_segment, 0, sizeof(task_state_segment));

	task_state_segment.rsp[0] = kernel_stack;
    

    uintptr_t addr = (uintptr_t)&task_state_segment - KERNEL_ADDR;

    gdt_entry_sys_t tss_entry = {}; //{0x0067, 0x0000, 0x00, 0xE9, 0x00, 0x00, 0x00000000, 0x00000000}; 
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.size = sizeof(task_state_segment) - 1; //unknown if sub 1 necessary or correct but seems to work with/without & we do for our gdtr so.. \_(0_0)_/ 
    tss_entry.base_0_15 =  (addr & 0xffff);
    tss_entry.base_16_23 = (addr >> 16) & 0xff;
    tss_entry.base_24_31 = (addr >> 24) & 0xff;
    tss_entry.base_31_64 = (addr >> 32) ; 

    tss_entry.access.accessed = 1; //access = 0x89 = 0b10001001
	tss_entry.access.RW = 0;
	tss_entry.access.DC = 0;
	tss_entry.access.exec = 1;
	tss_entry.access.not_sys = 0;
	tss_entry.access.priv = 0;
	tss_entry.access.present = 1;
    GDT* gdt = (GDT*)(GDT_PTR);
   // dump_gdt(gdt, 1);
    
    memcpy((void*)GDT_TSS_PTR, &tss_entry, sizeof(tss_entry));

	debugf("copied GDT tss to %lx", &kernel_stack); 
   // dump_gdt(gdt, 1);
	//so bochs now shows this as a 32 bit tss 
	// and then an unknown descriptor after it
	// which is probably fine ? bc our descriptor top half is empty ? 

   
}

extern void load_tss();
void init_descriptor_table(const uint8_t dump)
{
	debugf("==init GDT, TSS==\n");
	GDT* gdt = (GDT*)(GDT_PTR);
	init_gdt(gdt);
	
	if(dump == 1u) dump_gdt(gdt, 0);
		
		
	make_tss();
	
	load_tss(); //this reloads GDT and then tss
	debugf("==Reloaded GDT, Loaded TSS==\n");

	if(dump > 1u) dump_gdt(gdt, 1u);
	

	
	
}

