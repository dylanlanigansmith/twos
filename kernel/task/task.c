#include "task.h"
#include "../mem/heap.h"
#include "../../drivers/video/gfx.h"
#include "../timer/timer.h"

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
    uint64_t new_stack = kmalloc(KERNEL_TASK_STACK_SIZE) + KERNEL_TASK_STACK_SIZE - sizeof(task_t); 
    task_t* task = (task_t*)new_stack; //task goes up from stack addr, stack goes downwards, nice! 
    if(!new_stack) KPANIC("task allocation failed!");
   
    
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

    // handle list outside of create_task
    task->reserved = nullptr;
    task->next = nullptr;


    
    DEBUGT("create_task %s pid %i rip %lx !\n", name, task->pid, task->regs.rip);

    sched.tasks++;
    return task;
}




int tasking_init(task_entry_fn main_fn){
    sched.active = 0;
    DEBUGT("%s", "tasking init\n");
    sched.current_task = sched.newest_task = sched.next_switch = sched.pid_last  = sched.timer = 0;
    

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

    new_task->next =  sched.root_task;; //point new task to start of tasks since this is our top task 
                                                            //when another task is added this will be swapped with it, and so on 

   
    sched.newest_task = new_task;
    DEBUGT("scheduler: added task %s, pid = %i !\n", name, new_task->pid);

    
    return 0;
}


task_t* remove_current_task() //itrps are off for this
{


    if(sched.current_task == sched.root_task)
        KPANIC("tried to remove root task!"); //sometimes i write error checks that while plausible,
                                                    // it leaves me pondering how i may go down such a rabbithole to  get into the situation where they happen
                                                    //like if you are reading this, you really messed up future dylan
    
    //Remove our current task, aka exit()!
    //gotta update linked list !
    
    
    //starting at root task, go thru list until we find next entry pointing to current task
    task_t* task_old = sched.current_task;
    
    task_t* task = sched.root_task;

    uint32_t i = 0;
    while(task->next != task_old){
        if(task->next == 0){
            DEBUGT("scheduler: UHOH we found a null task in %s trying to remove %s\n",task_old->name, task->name); return;
        }
        if(i++ > (sched.tasks + 8) ){
            //dont worry about the 8 i only want this for if things get really screwed 
            //i just dont like not having a bailout 
            //except for now we crash the kernel anyways
            KPANIC("task_remove bailout!");
        }
        task = task->next;
    }

    //okay so we should have the task that points to the task we are removing now
    DEBUGT("found task %s pointing to del task %s\n", task->name, task_old->name);


    task->next = task_old->next; //set whatever task we are removing was pointing to, to the task that used to point to it

    //check for edges 
    if(sched.newest_task == task_old){
        sched.newest_task = task;
    }

    //uh i think that's actually it? 
    //could run thru tasks again to ensure its circular (last pts back to task_root) but i think its fine if it works


    sched.tasks--;

    //now task_old is free from our structure 


        //could use start task basically
        //or just int 0x20 with next_switch = 0
        // its likely whatever we do  wont work in user mode anyways! so its alll good

    sched.current_task = 0;
    sched.next_switch = 1; //avoid anything weird related to rollover due to past trauma 
    
    return task_old;
}

void start_first_task()
{
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
    if(sched.current_task != nullptr)
    {
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


        sched.current_task = sched.current_task->next;
    }
    else{
        //so we would have to set a flag or something not to dump regs for current task 
        // for now lets just restart from known truth (root task would never have been removed)
        //above solution will work fine, we will need more bitfield flags etc anyway
        sched.current_task = sched.root_task;
    }
    


    //WE SWITCHIN
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


    //oh shit oh fuck
}

void start_task(task_t *task) //kernel
{
    ASSERT(sched.current_task == 0);

    sched.current_task = task;
    
    sched.timer = tick;

    //HERE WE GO

     __asm__ volatile ("cli");
     sched.active = 1;
    restore_regs(&task->regs);
    __asm__ volatile ("sti"); //jump to usermode will fix flags for us 
   
}

PID_t getpid(){
    return sched.current_task->pid;
}


//__attribute__((noreturn)) 
void exit(int err){
    __asm__ volatile ("cli;");
    //called by task at end 
    DEBUGT("task %u exited with code: %i\n", sched.current_task->pid, err);

    heap_footer_t* task_foot = remove_current_task() + sizeof(task_t);
    
    if(task_foot->crc == HEAP_CRC){
        kfree(task_foot->header + sizeof(heap_header_t));
    }
   

    //eek
  
    
    __asm__ volatile ("sti; int 0x20") ; //do we need to interupt?
    for(;;){
        __asm__ volatile ("mov rax, 0xcafebabe; hlt;") ;
    }
}

void yield(){
    //task can voluntarily request to switch
    __asm__ volatile ("cli;");
    sched.next_switch = 0;
    __asm__ volatile ("sti; int 0x20"); //not sure 

}


