#include "syscall.h"

#include "../idt/isr.h"
#include "../task/task.h"
#include "../../drivers/video/gfx.h"
#include "../../drivers/port/port.h"
#include "../../drivers/input/keyboard.h"
#include "../timer/timer.h"
#include "sysinfo.h"
#include "../fs/initrd.h"
#include "../boot/multiboot2.h"
#include "../stdio/stdout.h"
void* syscall_test(registers_t* regs){
    print("your sis called ;)");

    return 0;
}


void* sys_print(registers_t* regs){
    if(regs->rdi != 0){
        char* str = (char*)regs->rdi;
       // debugf(" printing %s len = %li",str, strlen(str));
        print(str);
    }

    return 0;
}
void* sys_putc(registers_t* regs){
    putc((char)(regs->rdi & 0xff) );
  //  debugf("put char %c %i \n",  (char)(regs->rdi & 0xff), (char)(regs->rdi & 0xff));
    return 0;
}
void* sys_clearscreen(registers_t* regs){

    color clr; clr.argb = (uint32_t)regs->rdi;
    if(regs->rdi < 255) clr = gfx_state.clear_color;
   gfx_clear(clr);
    
    return (void*)0;
}
void* sys_setgfxmode(registers_t* regs){
    

    if(gfx_state.mode == 0){
        gfx_clear(color_white);
        gfx_state.mode = 1;
    }
    else{
        gfx_state.mode = 0;
        gfx_clear(color_cyan);
    } 
    //TODO

    return sysinfo.fb.addr;
}


void* sys_togglegamemode(registers_t* regs){
    
    keys_gamemode();
    //TODO

    return 0;
}



void* sys_exit(registers_t* regs){
    exit(regs->rdi);

    return 0; //never returns
}

void* sys_yield(registers_t* regs){
    yield();

    return 0; //never returns
}
void* sys_sleepms(registers_t* regs){
    if(regs->rdi == 0) return 1;
    task_sleep_ms(regs->rdi);

    return 0; //never returns
}


void* sys_getkeyhist(registers_t* regs){
    regs->rax = keys_getqueued();
    return (void*)regs->rax;
}

void* sys_getlastkey(registers_t* regs){
    regs->rax = keys_last_event();
    return (void*)regs->rax;
}

void* sys_iskeydown(registers_t* regs){
    uint8_t sc = (uint8_t)(regs->rax & 0xff) ;
    regs->rax = is_key_down(sc);
    return (void*)(regs->rax & 0xff);
}

void* sys_getcurtick(registers_t* regs){
   return (void*)tick;
}

void* sys_getlastkeytick(registers_t* regs){
   return (void*)last_key_event;
}

void* sys_exec(registers_t* regs){
    int err = 1;
    if(regs->rdi != 0){
        err = exec_user_task((char*)regs->rdi, regs);
    }

    return (void*)err;
}


void* sys_shutdown(registers_t* regs){
    if(sysinfo.host_type == HOST_BOCHS){
         debugf("shutting down via BOCHS method\n");
        port_word_out(0xB004, 0x2000);
        
    }
    else if(sysinfo.host_type == HOST_QEMU){
         debugf("shutting down via QEMU method\n");
        port_word_out(0x604, 0x0 | 0x2000);
    }
    else if(sysinfo.host_type == HOST_VM_OTHER){ //probs vbox
        debugf("shutting down via VBOX method on a whim\n");
         port_word_out(0x4004, 0x3400);
    }
    else {
        debugf("shutting down via stupid method\n");
        __asm__ volatile ("cli; lidt [rax]; int 6; lgdt [rax]"); //should do the trick
    }

   return 0;
}

void* sys_getgfx_info(registers_t* regs){
    //lets say usr has alloced a spot for this
    
    if(regs->rdi != 0){
        memset((gfx_info_t*)regs->rdi, 0, sizeof(gfx_info_t));
        gfx_get_info((gfx_info_t*)regs->rdi);
       
    }

    return sysinfo.fb.addr;
}

void* sys_getfsroot(registers_t* regs){ //does it deserve this tho

    initrd_demo();
   return initrd_root;
}


void* sys_requestheap(registers_t* regs){ //does it deserve this tho

   return initrd_root;
}

void* sys_brk(registers_t* regs){ 
    //AYO GIMME SOME MEMORY BRO
    size_t size = regs->rdi;

   void* r =  task_alloc_heap(size);
   
   
    return r;

}
const char* mmap_typess[6] = {"mem_unknown", "mem_avail", "mem_reserved", "mem_acpi_reclaimable", "mem_nvs", "mem_badram" };
void* sys_dumpmemmap(registers_t* regs)
{
     struct multiboot_tag_mmap *mmaps = (struct multiboot_tag_mmap *)sysinfo.mem.mb_map;

    sysinfo.mem.mb_map = mmaps;
  
    printf("===MULTIBOOT_TAG_TYPE_MMAP===\n");
    printf("mmaps->total_size=%i mmaps->entry_size=%i mmaps->entry_version=%i\n num_entries = %li\n",mmaps->size, mmaps->entry_size,  mmaps->entry_version,  sysinfo.mem.mb_size);
    //(size_t)( (sizeof(struct multiboot_tag_mmap) - sizeof(multiboot_memory_map_t)) + (mmaps->e)  
 
   
    for (int i = 0; i <  sysinfo.mem.mb_size; ++i)
    {

        multiboot_memory_map_t mmap = mmaps->entries[i];
        uint32_t type = (mmap.type > 5) ? 0 : mmap.type;
        printf("mmap [%i]:{ @%lx - %lx } size %lx |  %s %i \n", i,
               mmap.addr, mmap.addr + mmap.len, mmap.len, mmap_typess[type], mmap.type);
    }
    printf("fb @ %lx [%li kb] [%ix%i] %ip/%ibpp \n", sysinfo.fb.addr, BYTES_TO_MIB(sysinfo.fb.size), sysinfo.fb.w,  sysinfo.fb.h, sysinfo.fb.pitch, sysinfo.fb.bpp);
    return (void*)sysinfo.mem.mb_size;
}

void* sys_cls(registers_t* regs){
    stdout_clear();
    return 0;
}

void* sys_mmap(registers_t* regs){ 
    //AYO GIMME SOME MEMORY BRO
    const char* name = (char*)regs->rdi;

   void* r =  task_mmap_file(name);
   debug("mmaped that shit");
   
    return r;

}


typedef void* (*syscall_fn)(registers_t* regs);

#define SYSCALL_TOTAL 22 //this is stupid
static void* syscalls[] =
{
    &syscall_test, //0
    &sys_exit, 
    &sys_yield,
    &sys_sleepms,
    &sys_print, //4
    &sys_putc,
    &sys_clearscreen,
    &sys_setgfxmode,
    &sys_getlastkey, //8
    &sys_iskeydown,
    &sys_getcurtick,
    &sys_getlastkeytick, 
    &sys_shutdown, // 12
    &sys_exec,
    &sys_getfsroot,
    &sys_brk, //15
    &sys_mmap,
    &sys_getkeyhist,
    &sys_togglegamemode, //18
    &sys_getgfx_info,
    &sys_dumpmemmap,
    &sys_cls //21
};

static_assert(SYSCALL_TOTAL == (sizeof(syscalls) / sizeof(void*)), "SYSCALL TOTAL NOT UPDATED");


static void handle_syscall(registers_t* regs){

    if(regs->rax >= SYSCALL_TOTAL) return;

    syscall_fn call = syscalls[regs->rax];

    //how do we want to handle args
    /*
        something like make macros for syscall wrappers

        parse args (rdi) into fmt
        idk
    */
    void* ret = call(regs);

    regs->rax = ret;



}

void syscall_init()
{
    register_interupt_handler(SYSCALL_INT, &handle_syscall);
}