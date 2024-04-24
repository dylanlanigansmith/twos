#include "task.h"
#include "../mem/heap.h"
#include "../../drivers/video/gfx.h"
#include "../timer/timer.h"
#include "../pic/pic.h"
#include "elf.h"
#include "../fs/initrd.h"
#include "../pmm/pmm.h"

#define DEBUGT(fmt, ...) debugf(fmt, __VA_ARGS__)

extern void capture_regs(regs_t* regs);
extern void restore_regs(regs_t* regs);
extern void fix_isr_regs();



extern uint64_t get_cr3();


scheduler_t sched;

void print_regs(regs_t* regs)
{
    println("=======");
    printf("rsp: %lx\n", regs->rsp);
    printf("rbp: %lx\n", regs->rbp);
    printf("rip: %lx\n", regs->rip);
    printf("rax: %lx\n", regs->rax);
    printf("rbx: %lx\n", regs->rbx);
    printf("rcx: %lx\n", regs->rcx);
    printf("rdx: %lx\n", regs->rdx);
    printf("rsi: %lx\n", regs->rsi);
    printf("rdi: %lx\n", regs->rdi);
    printf("r8: %lx\n", regs->r8);
    printf("r9: %lx\n", regs->r9);
    printf("r10: %lx\n", regs->r10);
    printf("r11: %lx\n", regs->r11);
    printf("r12: %lx\n", regs->r12);
    printf("r13: %lx\n", regs->r13);
    printf("r14: %lx\n", regs->r14);
    printf("r15: %lx\n", regs->r15);
    printf("rflags: %lx\n", regs->rflags);
    printf("cr3: %lx\n", regs->cr3);
    println("=======");
}




task_t main_task, t1;

#define KERNEL_TASK_STACK_SIZE 4096 * 2
task_t* create_task( const char* name, task_entry_fn task_entry, uint64_t rflags) //ignore flags and vma for now 
{
    uintptr_t mem = kmalloc(KERNEL_TASK_STACK_SIZE);
    uint64_t new_stack = mem + KERNEL_TASK_STACK_SIZE - sizeof(task_t); 
    task_t* task = (task_t*)new_stack; //task goes up from stack addr, stack goes downwards, nice! 
    if(!new_stack) KPANIC("task allocation failed!");
    memset((void*)task, 0, sizeof(task_t));
    task->mem = mem ; //bc we keep losing these...
    //we should ensure this is aligned!!!!!!
    if(new_stack & 0xf){
        DEBUGT("stack  %lx isn't aligned! repairing to = %lx \n", new_stack, new_stack - (new_stack % 16llu));
        new_stack = new_stack - (new_stack % 16llu);
    } 

    strncpy(task->name, name, 31);
    task->pid = sched.pid_last++;

    
    //maybe this should just go in task alloc

    task->regs.rsp = new_stack; //these arent 16 bit aligned!!! 
    task->regs.rbp = new_stack;
    task->regs.rip = (uint64_t)task_entry;
    task->regs.rax = 0;
    task->regs.rbx = 0;
    task->regs.rcx = 0;
    task->regs.rdx = 0;
    task->regs.rsi = 0;
    task->regs.rdi = 0;
    task->regs.r8 = 0;
    task->regs.r9 = 0;
    task->regs.r10 = 0;
    task->regs.r11 = 0;
    task->regs.r12 = 0;
    task->regs.r13 = 0;
    task->regs.r14 = 0;
    task->regs.r15 = 0;
    task->regs.rflags = rflags;
    task->regs.cr3 = get_cr3();

    ASSERT(task->regs.cr3);

    task->ds = 0x10; 
    task->cs = 0x8;
    // handle list outside of create_task
    
    task->parent_task = 0;

    task->next = nullptr;


    task->maps.heap_phys = 0;
    task->maps.heap_size = 0;
    task->maps.heap_virt = 0;
    DEBUGT("create_task %s pid %i rip %lx !\n", name, task->pid, task->regs.rip);


    task->parent_PID = TASK_ORPHAN;
    task->flags.sleeping = task->flags.is_user = task->flags.blocks_parent = 0;
 
    sched.tasks++;
    return task;
}




int tasking_init(task_entry_fn main_fn){
    sched.active = 0;
    DEBUGT("%s", "tasking init\n");
    sched.current_task = sched.newest_task = sched.next_switch = sched.pid_last  = sched.timer = 0;
    
    sched.skip_saving_regs = False;
    regs_t r; capture_regs(&r); //for eflags and cr3 initial
    
    sched.defaults.cr3 = r.cr3;
    sched.defaults.rflags = r.rflags;
    
    

    
    sched.pid_last = 0; 
   
    //add our first task
    sched.root_task = create_task("task_kmain", main_fn, main_task.regs.rflags);
    
    sched.newest_task = sched.root_task;
   
  
    DEBUGT("added initial task %s pid %i !\n", sched.root_task->name, sched.root_task->pid);
    return 0;
}

int add_task(const char* name, task_entry_fn main_fn) //note that this isnt safe to run while scheduler is active! must fix
{
    DEBUGT("adding task %s, pid_last = %i !\n", name, sched.pid_last);
    

    task_t* new_task = create_task(name, main_fn, sched.defaults.rflags);
    

    

    sched.newest_task->next = new_task;

    new_task->next =  sched.root_task; //point new task to start of tasks since this is our top task 
                                                            //when another task is added this will be swapped with it, and so on 

   
    sched.newest_task = new_task;
    DEBUGT("scheduler: added task %s, pid = %i !\n", name, new_task->pid);

    
    return 0;
}

void* copy_usr_vas_info(task_t* task, user_vas_t *usr){
    task->vma = kmalloc(sizeof(user_vas_t));
    ASSERT(task->vma);

    return memcpy(task->vma, (void*)usr, sizeof(user_vas_t));
}


int add_user_task(const char *name, user_vas_t *usr)
{
    DEBUGT("adding USER task %s, pid_last = %i !\n", name, sched.pid_last);
    
    task_entry_fn entry = usr->entry;
    task_t* new_task = create_task(name, entry, sched.defaults.rflags); //our stack is gonna be wrong
    
    new_task->regs.rsp = usr->stack.top;
    new_task->regs.rbp = usr->stack.top;
    
    new_task->regs.cr3 = (uintptr_t)(usr->pt.p4p); //p4 physical
    new_task->cs = 0x1b; new_task->ds = 0x23;

   

    new_task->flags.is_user = 1;

    sched.newest_task->next = new_task;

    new_task->next =  sched.root_task;; //point new task to start of tasks since this is our top task 
                                                            //when another task is added this will be swapped with it, and so on 

    copy_usr_vas_info(new_task, usr);
    sched.newest_task = new_task;
    DEBUGT("scheduler: added USER task %s, pid = %i !\n", name, new_task->pid);

    
    return 0;
}

task_t* remove_current_task() //itrps are off for this
{
     //Remove our current task, aka exit()!

    if(sched.current_task == sched.root_task)
        KPANIC("tried to remove root task!"); //sometimes i write error checks that while plausible,
                                                    // it leaves me pondering how i may go down such a rabbithole to  get into the situation where they happen
                                                    //like if you are reading this, you really messed up future dylan
    
   
    //starting at root task, go thru list until we find next entry pointing to current task
    task_t* task_old = sched.current_task;
    
    task_t* task = sched.root_task;

    uint32_t i = 0;  
    while(task->next != (void*)task_old){
        if(task->next == 0){
            DEBUGT("scheduler: UHOH we found a null task in %s trying to remove %s\n",task_old->name, task->name); return 0;
        }
        if(i++ > (sched.tasks + 8) ){
            //dont worry about the 8 i only want this for if things get really screwed 
            //i just dont like not having a bailout 
            //except for now we crash the kernel anyways
            KPANIC("task_remove bailout!");
        }
        task = (task_t*)task->next;
    }

    //okay so we should have the task that points to the task we are removing now
    DEBUGT("found task %s pointing to del task %s\n", task->name, task_old->name);


    task->next = task_old->next; //set whatever task we are removing was pointing to, to the task that used to point to it

    //check for edges 
    if(sched.newest_task == task_old){
        sched.newest_task = task;
        DEBUGT("set newest task to other as we were it, now %s\n", sched.newest_task->name);
    }

    //uh i think that's actually it? 
    sched.tasks--;

    //now task_old is free from our structure 

    sched.current_task = task_old->next; //go to where we were gonna anyyways 
    sched.skip_saving_regs = True;
    sched.next_switch = 1; //avoid anything weird related to rollover due to past trauma 
    


    if(task_old->parent_PID != TASK_ORPHAN){
        //we have a parent!
        if(task_old->flags.blocks_parent){
            if(task_old->parent_task != nullptr){
                task_t* parent = (task_t*)(task_old->parent_task);
                if(parent->flags.sleeping){
                    parent->flags.sleeping = 0u;
                     debugf("remove_current_task(): woke up parent (%i) of ending task (%i)...\n", task_old->parent_PID, task_old->pid);
                }
            }
            else{
                debugf("remove_current_task(): our removed task has a parent and we cant find it...\n");
                //get task from pid fn goes here
            }
        }
    }


    return task_old;
}

void start_first_task()
{
    debugf("starting first task %s", sched.root_task->name);
    sched.next_switch = tick + ARBITRARY_SWITCH_INTERVAL;
    sched.timer = tick;
    start_task(sched.root_task);

   
}

void on_timer_tick(uint64_t ticks, registers_t* reg) //dont get me started on the two types of registers_t ok. 
{
    
    
    if(ticks < sched.next_switch) return;
    
    //also assert next task exists before we switch

    sched.next_switch = ticks + ARBITRARY_SWITCH_INTERVAL;
    sched.timer = ticks;

    if(sched.current_task != nullptr && sched.skip_saving_regs == False)
    {
    
        //imagine not having two almost identical structs???
        //couldnt be me 
        //id fix this but then id have to go change all the offsets in the asm code and...
        sched.current_task->regs.rbp = reg->rbp;
        sched.current_task->regs.rsp = reg->rsp;
        sched.current_task->regs.rip = reg->rip;
        sched.current_task->regs.rflags = reg->rflags;

        sched.current_task->regs.rax = reg->rax;
        sched.current_task->regs.rcx = reg->rcx;
        sched.current_task->regs.rdx = reg->rdx;
        sched.current_task->regs.rbx = reg->rbx;
        sched.current_task->regs.rdi = reg->rdi;
        sched.current_task->regs.rsi = reg->rsi;

        sched.current_task->regs.r9 = reg->r9;
        sched.current_task->regs.r10 = reg->r10;
        sched.current_task->regs.r11 = reg->r11;
        sched.current_task->regs.r12 = reg->r12;
        sched.current_task->regs.r13 = reg->r13;
        sched.current_task->regs.r14 = reg->r14;
        sched.current_task->regs.r15 = reg->r15;

        sched.current_task->cs = reg->cs;
        sched.current_task->ds = reg->ds;

        sched.current_task = sched.current_task->next;
    }
    else{
        //so we would have to set a flag or something not to dump regs for current task 
        // for now lets just restart from known truth (root task would never have been removed)
        //above solution will work fine, we will need more bitfield flags etc anyway
        if ( sched.current_task  == nullptr)
            sched.current_task = sched.root_task;
        if( sched.skip_saving_regs){
            sched.skip_saving_regs = False;
        }
    }
    if(sched.current_task->flags.sleeping == 1u){
            //check if wakey time

            if(sched.current_task->parent_PID != TASK_ORPHAN){
                if(!sched.current_task->flags.blocks_parent){
                    //check if wakeup time
                }
            }
            //we sleepy skip us
            //debugf("skipping sleeping task %i -> %i", sched.current_task->pid, (task_t*)(sched.current_task->next)->pid);
            sched.current_task =  sched.current_task->next;
            if(sched.current_task == nullptr)
                sched.current_task = sched.root_task;
    }
   
   
    reg->ss = sched.current_task->ds;
    //WE SWITCHIN
    reg->cs = sched.current_task->cs;
    reg->ds = sched.current_task->ds;
    reg->rbp = sched.current_task->regs.rbp;
    reg->rsp = sched.current_task->regs.rsp;
    reg->rip = sched.current_task->regs.rip;
    reg->rflags = sched.current_task->regs.rflags;

    reg->rax = sched.current_task->regs.rax;
    reg->rcx = sched.current_task->regs.rcx;
    reg->rdx = sched.current_task->regs.rdx;
    reg->rbx = sched.current_task->regs.rbx;
    reg->rdi = sched.current_task->regs.rdi;
    reg->rsi = sched.current_task->regs.rsi;

    

    reg->r9 = sched.current_task->regs.r9;
    reg->r10 = sched.current_task->regs.r10;
    reg->r11 = sched.current_task->regs.r11;
    reg->r12 = sched.current_task->regs.r12;
    reg->r13 = sched.current_task->regs.r13;
    reg->r14 = sched.current_task->regs.r14;
    reg->r15 = sched.current_task->regs.r15;
    reg->cr3 = sched.current_task->regs.cr3;
    //oh shit oh fuck
    //we boutta be in a different task
}

void start_task(task_t *task) //kernel
{
    ASSERT(sched.current_task == 0);

    sched.current_task = task;
    
    sched.timer = tick;
    debugf("starting task %s", task->name);
    //HERE WE GO
     PIC_set_mask(0x20);
     sched.active = 1;
     
    restore_regs(&task->regs);
  
    PIC_clear_mask(0x20); 
   
}

PID_t getpid(){
    return sched.current_task->pid;
}

void sleep_ns(uint64_t ns)
{
    //TODO
}


extern swap_cr3(uintptr_t);

int exec_user_task(const char *taskname, registers_t* reg)
{
    //since this was called in a syscall we shouldnt get interupted right?
    //right???
    char* name = kmalloc(strlen(taskname)); //we cant touch processes address space once we swap cr3s!!!! TODO MOVE F+UCKIN STACKS

    strcpy(name, taskname);
    ASSERT(initrd_root);
    vfs_node* bin = initrd_findfile(initrd_root, name); //bad
    if(!bin){
        debugf("exec_user_task(): Failed to find binary %s", name); return 1;
    }
   
      uintptr_t prev_cr3 = get_cr3();
    uintptr_t old_cr3 = swap_cr3(sched.root_task->regs.cr3); //steal known kernel task cr3
      uintptr_t new_cr3 = get_cr3();
    debugf("exec_user_task(): old cr3 %lx new cr3 %lx insane %lx wtf %lx", prev_cr3, new_cr3,old_cr3, (uintptr_t)name);
    user_vas_t usr; memset(&usr, 0, sizeof(user_vas_t));

    if(load_elf(bin, &usr)){
         debugf("exec_user_task(): load elf64 %s failure", name); return 1;
    }

    debugf("exec_user_task():\n     usr entry = %lx usr phys = %lx usr lo %lx usr rsp %lx, usr cr3 = %lx",usr.entry, usr.phys, usr.vaddr.l, usr.stack.top, usr.pt.p4p);
    
   

    //so the calling PID shouldnt run until this guy is over 
    DEBUGT("adding USER task %s, child of %i !\n", name, sched.current_task->pid);
    
    

    task_entry_fn entry = usr.entry;
    task_t* new_task = create_task(name, entry, sched.defaults.rflags); //note that yr still doing greasy stack thing
    
    new_task->regs.rsp = usr.stack.top;
    new_task->regs.rbp = usr.stack.top;
    
    new_task->regs.cr3 = (uintptr_t)(usr.pt.p4p); //p4 physical
    new_task->cs = 0x1b; new_task->ds = 0x23;

    new_task->flags.is_user = 1;
    sched.newest_task->next = new_task;


    new_task->next =  sched.root_task;; //point new task to start of tasks since this is our top task 
                                                            //when another task is added this will be swapped with it, and so on 

    sched.newest_task = new_task;

    new_task->flags.blocks_parent = 1u;
    new_task->parent_PID = sched.current_task->pid;
    new_task->parent_task = sched.current_task;
    copy_usr_vas_info(new_task, &usr);
    sched.current_task->flags.sleeping = 1u;


    DEBUGT("scheduler: added child user task %s, pid = %i !\n", name, new_task->pid);

   // kfree(name); //hhahahahhahahahahahahahahahahahahahaha *cries*

    //swap back cr3, pray
    debugf("scheduler: swapping back cr3 to %lx", sched.current_task->regs.cr3);
    swap_cr3(sched.current_task->regs.cr3); 

    sched.skip_saving_regs = True;
   //manually dump regs
    sched.current_task->regs.rbp = reg->rbp;
    sched.current_task->regs.rsp = reg->rsp;
    sched.current_task->regs.rip = reg->rip;
    sched.current_task->regs.rflags = reg->rflags;

    sched.current_task->regs.rax = reg->rax;
    sched.current_task->regs.rcx = reg->rcx;
    sched.current_task->regs.rdx = reg->rdx;
    sched.current_task->regs.rbx = reg->rbx;
    sched.current_task->regs.rdi = reg->rdi;
    sched.current_task->regs.rsi = reg->rsi;

    sched.current_task->regs.r9 = reg->r9;
    sched.current_task->regs.r10 = reg->r10;
    sched.current_task->regs.r11 = reg->r11;
    sched.current_task->regs.r12 = reg->r12;
    sched.current_task->regs.r13 = reg->r13;
    sched.current_task->regs.r14 = reg->r14;
    sched.current_task->regs.r15 = reg->r15;

    sched.current_task->cs = reg->cs;
    sched.current_task->ds = reg->ds;
    yield();
    return 0;
}



//__attribute__((noreturn)) 
void exit(int err){
    __asm__ volatile ("cli;");
    if(!sched.current_task || !sched.active){
        return start_first_task();
    }

    //called by task at end 
    DEBUGT("task %u exited with code: %i\n", sched.current_task->pid, err);

   task_t* old = remove_current_task() ;

    debugf("old parent pid = %i", old->parent_PID);
   // kfree(old->mem); //used to have a smart way of doing this, now we have 8 extra bytes and a dumb way
    //yeah our heap is corrupt!
   
   // debugf("task removed, next task now %i", sched.);



  
    
    __asm__ volatile ("sti; int 0x20") ; //do we need to interupt?
    for(;;){
        __asm__ volatile ("mov rax, 0xcafebabe; hlt;") ;
    }
}

void yield(){
    //task can voluntarily request to switch
    ASSERT(sched.current_task->next);
    __asm__ volatile ("cli;");
    sched.next_switch = 0;
    __asm__ volatile ("sti; int 0x20"); //not sure 

}

#define USR_HEAP 0xfffffffd00000000

void* task_expand_heap(task_t* task, size_t req_size)
{
    debugf("expanding heap for task %i [%li]", task->pid, req_size);
    
    size_t size = round_up_to_page(req_size); 
    int pages = calc_num_pages(size);

    //lets get some memory

    uintptr_t phys = pmm_alloc(size);


    user_vas_t* usr = (user_vas_t*)(task->vma);
    //ok where we gonna put it    
     uintptr_t old_cr3 = swap_cr3(sched.root_task->regs.cr3);
    
    ASSERT(map_phys_addr(usr->vaddr.l, usr->phys,usr->size , PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER)); //map usr pages
    
    uintptr_t virt = task->maps.heap_virt + task->maps.heap_size; 

    ASSERT(map_user_page_tables(virt, phys, size, &usr->pt, 1));

    unmap_phys_addr(usr->vaddr.l, usr->size ); //unmap usr pages from kernel
    
    
    task->maps.heap_size += size;
   
    
   // PIC_disable();
    // __asm__ volatile ("sti; int 3");
    debugf("expanded heap for task %i:\n        pa %lx va%lx size %li kb \n cr3 to restore: %lx \n", task->pid, task->maps.heap_phys, task->maps.heap_virt, BYTES_TO_KIB( task->maps.heap_size ), old_cr3);
    
    swap_cr3(sched.current_task->regs.cr3);
    
    

  
    return (void*)virt;

}

void *task_alloc_heap(size_t req_size)
{
    task_t* task = sched.current_task;
    if(!task->flags.is_user) return 0; //damn kernel use your own heap!

    ASSERT(task->vma);

    if(task->maps.heap_size > 0){
        return task_expand_heap(task, req_size);
    }
    debugf("allocated a heap for task %i\n", task->pid);
    size_t size = round_up_to_page(req_size); 
    int pages = calc_num_pages(size);

    //lets get some memory

    uintptr_t phys = pmm_alloc(size);


    user_vas_t* usr = (user_vas_t*)(task->vma);
    //ok where we gonna put it    
     uintptr_t old_cr3 = swap_cr3(sched.root_task->regs.cr3);
    
    ASSERT(map_phys_addr(usr->vaddr.l, usr->phys,usr->size , PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER));
    
    uintptr_t virt = round_up_to_page( usr->size + PAGE_SIZE * 2); //called the idiot gap

    ASSERT(map_user_page_tables(virt, phys, size, &usr->pt, 1));

    unmap_phys_addr(usr->vaddr.l, usr->size ); //from kernel
    
    task->maps.heap_phys = phys;
    task->maps.heap_size = size;
    task->maps.heap_virt = virt;
    
   // PIC_disable();
    // __asm__ volatile ("sti; int 3");
    debugf("allocated a heap for task %i:\n        pa %lx va%lx size %li kb \n cr3 to restore: %lx \n", task->pid, task->maps.heap_phys, task->maps.heap_virt, BYTES_TO_KIB( task->maps.heap_size ), old_cr3);
    
    swap_cr3(sched.current_task->regs.cr3);
    
    

  
    return (void*)virt;

}

void *task_mmap_file(const char *name)
{
    vfs_node* file = initrd_findfile(initrd_root, name); //bad
    if(!file){
        debugf("\n\n\nmmap_file(): Failed to find file %s", name); return 1;
    }

     task_t* task = sched.current_task;
    if(!task->flags.is_user) return 0; //damn kernel use your own heap!

    ASSERT(task->vma);

    size_t size = round_up_to_page(file->length); 
    int pages = calc_num_pages(size);

    //lets get some memory
    debugf("\n task_mmap_file(): %s to be mmaped in %i pages from %lx", file->name, pages, file->addr);
    uintptr_t phys = pmm_alloc(size);


    user_vas_t* usr = (user_vas_t*)(task->vma);
    //ok where we gonna put it    
     uintptr_t old_cr3 = swap_cr3(sched.root_task->regs.cr3);
         
    //fuck it stick it at one gib until we ever need to mmap more than one file
    uintptr_t virt = ONE_GIB  / 2;
    ASSERT(map_phys_addr(usr->vaddr.l, usr->phys,usr->size , PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER));
    
    ASSERT(map_phys_addr(virt, phys, size, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_USER));
    memcpy(virt, file->addr, file->length);

    ASSERT(map_user_page_tables(virt, phys, size, &usr->pt, 1));

    unmap_phys_addr(usr->vaddr.l, usr->size ); //from kernel
    unmap_phys_addr(virt, size);

    
   // PIC_disable();
    // __asm__ volatile ("sti; int 3");
    debugf("task %i has file len='%li' mmaped at va %lx pa %lx size %li kb", task->pid,file->length, virt, phys, BYTES_TO_KIB( size ));
    
    swap_cr3(sched.current_task->regs.cr3);

    return (void*)virt;
}


//stack is running into page tables
//pmm sus
