#include "../drivers/video/console.h"
#include "stdlib.h"
#include "../drivers/port/port.h"
#include "idt/idt.h"
#include "idt/isr.h"
#include "pic/pic.h"
#include "timer/timer.h"
#include "../drivers/input/keyboard.h"
#include "../drivers/video/gfx.h"
#include "../drivers/video/vbe.h"
#include "../common/scancode.h"
#include "../drivers/sound/sound.h"
#include "boot/multiboot2.h"
#include "boot/mb_header.h"
#include "boot/cmdline.h"
#include "sys/sysinfo.h"
#include "sys/syscall.h"
#include "../drivers/serial/serial.h"

#include "mem/page.h"
#include "mem/heap.h"

#include "task/task.h"
#include "task/elf.h"
#include "stdio/stdout.h"
#include "gdt/gdt.h"

#include "mem/page_alloc.h"

#include "acpi/madt.h"

#include "fs/initrd.h"

#include "pmm/pmm.h"


int cpp_test(int, int);




uint64_t start_tick = 0;

bool first_call = False;

uint64_t last_tick = 0;
void task_drawtimer(){
    
    serial_print("task draw timer \n");
     gfx_clear_line(738, 24);
    for(;;){
        if (gfx_state.mode == 1){ //usermode has fb
            task_sleep_ms(5000);
            yield();
        }
        else if((last_tick + 100) < tick  ){
             //   serial_printi("t=", tick);
                 gfx_clear_line(738, 24);
                last_tick = tick;
                gfx_print_pos(lltoa(tick, 10), v2(5, 740));

                uint8_t sc = keys_last_event();
                if(sc){
                    gfx_print_pos(itoa(sc, 10), v2(200, 740));
                    gfx_print_pos((sc & PS2_MAKEORBREAK) ? "UP" : "DOWN", v2(230, 740));
                    gfx_print_pos(get_scancode_name(sc & 0x7f), v2(265, 740));
                    
                }
                uint64_t rax = 0;
               //  __asm__ volatile ( "mov rax, 3; int 0x69; mov %0, rax; " : "=r"(rax) ::  "rax") ;

            }
    }
}
uint64_t last_tick2 = 0;
void task_drawstdout(){
    
    serial_print("task draw test \n");
    for(;;){
        if (gfx_state.mode == 1){ //usermode has fb
            task_sleep_ms(5000);
            yield();
        }
        else if((last_tick2 + 100) < tick  ){

            if(stdout_dirty()){
                 gfx_clear_text();
                // __asm__ volatile ("cli"); //should just mask PICs
                  PIC_disable();
                  gfx_print(get_stdout());
                  stdout_flush();
                  PIC_enable();
                //  __asm__ volatile ("sti");
            }
             last_tick2 = tick; //might break
     
            }
    }
}

void task_exit_test(){
    register uint64_t last = tick;
    serial_print("task exit test \n");
    
    exit(0);
}





void task_test_yield(){
     register uint64_t last = tick;
     serial_print("yield test");
     yield();
     for(;;){
        if((last + 1000) < tick  ){

            
             
            serial_print("yielding");

            last = tick;
         //   gfx_clear(gfx_state.clear_color);
           
             //   serial_printi("t=", tick);
              //  gfx_clear_line(498, 24);
               // last_tick2 = tick;
                //gfx_print_pos(lltoa(tick, 10), v2(5, 500));
        }
        if(tick > 5000) {
            serial_print("yield test over");
            exit(0);
        }
    }
}


extern uint64_t has_cpuid();

extern __attribute__((noreturn)) void jump_to_usermode(void* addr, uintptr_t stack, uintptr_t cr3);


void main(void *addr, void *magic)
{
    
    disable_interupts();
   
    if(get_multiboot_initial_info(addr, magic) == MB_HEADER_PARSE_ERROR && 0){
        //uh what?
        serial_init(); debugf("mb2 header parse error. magic =%lx", magic); return;
        //we can at least try and do something, this is why we need to implement bulletproof printk
    }
    if(cmdline_numargs())
    {
        if(cmdline_is_true("serial")){
            serial_init(); //we can now debug and log! 
            if(cmdline_is_true("dbg")) //eventually use as verbositiy level
            {
                if(port_byte_in(0xe9) == 0xe9){
                    serial_set_e9();
                    debug("Using Bochs E9 Hack for debug out\n"); //bochs
                    sysinfo.host_type = HOST_BOCHS;
                } else{
                    debug("using Serial COM1 for debug out \n"); //qemu (superior)
                    sysinfo.host_type = HOST_QEMU; //BAD BAD BAD BAD ABD
                } //todo: https://wiki.osdev.org/Parallel_port and use that for kernel debug
            }
        }
       
    }
     /*
        we should try and bootstrap up printk like linux does
            - callable in any context! 
        aka parse multiboot ASAP and see if we can get a frame buffer going - otherwise buffer printk's and send to serial as well
    */    
   
    debugf("boot start\n");

    //should check multiboot here while we have a working system

   
   
   

    //todo: APIC 
    PIC_init();
   
    //our old friend
    init_idt();
    
    // register irq handlers
    keyboard_init();
    timer_init(PIT_RATE);

    syscall_init();
     init_descriptor_table(GDT_INIT_QUIET); //update gdt
                            //add user mode segments 
                            //add tss


    debugf("interupts initialized\n==boot initial phase complete==\n\n"); 
   
    debugf("CPUID available = %s\n", (has_cpuid()) ? "YES" : "NO WTF");
    //so we gotta do this sooner in boot stage! 
    if(parse_multiboot_header(addr, (uint64_t)magic) == MB_HEADER_PARSE_ERROR){ //gets acpi, framebuffer, etc
        KPANIC("multiboot parse fail");
    }else{
        //do shit like are we uefi, what vm is this etc etc etc 
    }


    //we are doing the new and improved... lower half kernel!!!!

    //aka i dont wanna do higher half, i dont like it the bootstrapping required
    //so we will try and keep the kernel under 1gb
    pmm_init();
    initrd_reserve_space(sysinfo.initrd.start, sysinfo.initrd.end - sysinfo.initrd.start);
    
    
   
    paging_init(); //this also initializes heap, maps frame buffer, page allocator, etc.
    

    
    //no cpp in kernel atm
    _init_cpp(); //todo fix global constructors and actually write some cpp
                    //or just save it for user space where it belongs :))))
    debugf("skipping acpi\n");
    if(sysinfo.rsdp && 0){  

        uintptr_t rsdt = map_phys_addr(ACPI_ADDR, sysinfo.rsdp->RsdtAddress, PAGE_SIZE * 2, 0b10000001LLU ) ;
                                        // base mapping and offset from it 
        ACPI_discover_SDTs( (void*)(rsdt), ACPI_ADDR, sysinfo.rsdp->RsdtAddress);

        madt_discover();
        //todo: apic

    }
    debugf("\n==MEM INIT OK==\n");

    io_wait(); io_wait(); io_wait();
    __asm__ volatile ("sti");
    if(sysinfo.boot.is_uefi){
        //PIC_sendEOI();
        PIC_enable(); //as if we even have these
    }


    debugf("enabled interupts.\n==boot second phase complete==\n"); 


   // __asm__ volatile ("int 1; int 32");
    
     
    void* lol = kmalloc(69); //catch early if we break kernel heap somehow
    kfree(lol); //this is so fucked

    debugf("loading initial ramdisk... \n");
    if(initrd(sysinfo.initrd.start, sysinfo.initrd.end - sysinfo.initrd.start) != 0){
        //shit that sucks man

    }
    
    


    vfs_node* elf = initrd_findfile(initrd_root, "init");

    user_vas_t usr; memset(&usr, 0, sizeof(user_vas_t));
    if(elf){
       load_elf(elf, &usr);
       debugf(" usr entry = %lx usr phys = %lx usr lo %lx usr rsp %lx, usr cr3 = %lx",usr.entry, usr.phys, usr.vaddr.l, usr.stack.top, usr.pt.p4p);
    }
    stdout_init();
    
    gfx_init(color_cyan);
    //we are so back
     
    void* aghhhhhhh = pmm_alloc(PAGE_SIZE * 16 + 32);
   
    
    //uh wait it worked?
    /*
        todo:
        - i would like user VAS/page tables [x]
        - get user tasking to work [x]
        - make a user program with gfx etc
        - make basic libc
        -PORT DOOM 
    
    */    

   

    tasking_init(&task_drawtimer); //first task should be on our main stack as its new kernel main 
    add_task("task_drawstdout",task_drawstdout);
    add_task("task_exit_test",task_exit_test);
    add_task("task_yield_test",task_test_yield);

    add_user_task("usermode", &usr);
    //we have gotten ourselves a system with processes running and usermode
    // do a little dance or something 

    
    ASSERT(gfx_has_init());
    print("randos up \n");
   

    
   

    
    start_first_task();
    
   //if we arent doing tasking for some reason

   //heres a left over demo from initrd bringup
    uint64_t last_tick = 0;

    initrd_demo();
       
    vfs_node* file = initrd_findfile(initrd_root, "ulysses.txt");
    if(file){
        vfs_printfile(file, 512);
    }
    uint64_t sc = tick;
    size_t offset = 512;

    for(;;){
     
        if (stdout_dirty() ){ 
           
          
            gfx_clear_text();
        
            gfx_print(get_stdout());
        
           
            stdout_flush();

         
        }

        if(tick > sc + 2000 ){
            stdout_clear();
             vfs_printfile_at(file, offset, 2048);
             offset += 1024;
             sc = tick;
        }
        
    }

    // kernel ends, we can return to entry pt which hangs or just do it here for transparency while we develop
    __asm__("cli");
    for (;;)
    {
        __asm__("hlt");
    }
    __builtin_unreachable();
}

