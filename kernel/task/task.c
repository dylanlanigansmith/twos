#include "task.h"
#include "../mem/heap.h"
#include "../../drivers/video/gfx.h"
#include "../timer/timer.h"

#define DEBUGT(fmt, ...) printf(fmt, __VA_ARGS__)

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
    if(!new_stack) panic("task allocation failed!");
    if(strlen(name) >= 31) panic("you only made task names 31 chars long!");
        

    task->pid = sched.pid_last + 1;

    
    //maybe this should just go in task alloc

    task->regs.rsp = new_stack;
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
    return task;
}




int tasking_init(task_entry_fn main_fn){
    DEBUGT("%s", "tasking init\n");
    sched.current_task = sched.next_switch = sched.pid_last = sched.tasks = sched.timer = 0;
    regs_t r; capture_regs(&r); //for eflags and cr3 initial
    
    sched.defaults.cr3 = r.cr3;
    sched.defaults.rflags = r.rflags;
    
    sched.tasks = kmalloc(sizeof(task_t) * MAX_TASK + 1);
    ASSERT(sched.tasks);

    //add our first task
    sched.tasks[0] = create_task("task_drawtimer", main_fn, main_task.regs.rflags);
    
    sched.tasks[0]->pid = 0;
    sched.pid_last = 0; //this surely wont cause some stupid bug bc its dumb
    //nah it will
     DEBUGT("added initial task %s pid %i !\n", sched.tasks[0]->name, sched.tasks[0]->pid);
    return 0;
}

int add_task(const char* name, task_entry_fn main_fn)
{
    DEBUGT("adding task %s pid %i !\n", name, sched.pid_last + 1);
    ASSERT(sched.pid_last < MAX_TASK);

    sched.tasks[sched.pid_last + 1] = create_task(name, main_fn, sched.defaults.rflags);

    //lazy hack for testing

    sched.tasks[0]->next =  sched.tasks[sched.pid_last + 1];

    sched.tasks[sched.pid_last + 1]->next = sched.tasks[0]; // point our two test tasks at each other 


    return 0;
}
#define ARBITRARY_SWITCH_INTERVAL 100
void start_first_task()
{
    sched.next_switch = tick + ARBITRARY_SWITCH_INTERVAL;
    sched.timer = tick;
    start_task(sched.tasks[0]);

   
}

void on_timer_tick(uint64_t ticks, registers_t* reg) //dont get me started on the two types of registers_t ok. 
{
    //okay so if this shit gets called during an intrp like i feel like its problematic vs just swapping on tick which actually sucks fr 

    if(ticks < sched.next_switch) return;

    sched.next_switch = ticks + ARBITRARY_SWITCH_INTERVAL;
    sched.timer = ticks;


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

void start_task(task_t *task)
{
    ASSERT(sched.current_task == 0);

    sched.current_task = task;
    
    sched.timer = tick;

    //HERE WE GO

    task_entry_fn fn = (task_entry_fn)(task->regs.rip);
    //yeah no way hose
     __asm__ volatile ("cli");
    restore_regs(&task->regs);
    __asm__ volatile ("sti ;  ");
    //fn(); 
}

void switchTask(task_t* prev, task_t* new){
    //yk i love that you can use new as a name in c
    uint64_t return_address =  __builtin_return_address (1);
    printf("return address: %lx", return_address);
   
    capture_regs(&prev->regs);
   // print_regs(&new->regs);
    prev->regs.rip = return_address;
   //  main_task.regs.rip = &test;
    restore_regs(&(new->regs));
    
    //no fuckin way
}
bool first = False;
void yield(){
    
    
    task_t* prev = 0; //task_current;
    ASSERT(prev != nullptr);

  //  task_current = task_current->next;
   // switchTask(prev, task_current);
   //yk i love that you can use new as a name in c
    uint64_t return_address =  __builtin_return_address (0);
   // if(first) gfx_clear(color_red);
   // printf("return address: %lx", return_address);
    __asm__ volatile ("cli");
    capture_regs(&prev->regs);
    prev->regs.rip = return_address;
    
   // print_regs(&prev->regs);
   // print_regs(&task_current->regs);
   // if(first) panic("hm");
    first = True;
   //  main_task.regs.rip = &test;
  //  restore_regs(&task_current->regs);
    
    __asm__ volatile ("sti");
    
}

PID_t getpid(){
    return sched.current_task->pid;
}



int fork()
{
    return 0;
}
