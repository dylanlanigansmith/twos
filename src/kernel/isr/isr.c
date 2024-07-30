#include <kernel/kernel.h>
#include <kernel/isr/isr.h>
#include <kernel/printk.h>
#include <kernel/isr/isr_exceptions.h>

#include <kernel/apic/apic.h>

isr_t interupt_handlers[256];



extern uintptr_t get_cr2();

void print_isr_regs(registers_t* regs, bool extended)
{
    uintptr_t cr2 = get_cr2();
    debugf("===isr[%lx] err{%lx}====\n", regs->int_no, regs->err_code);
    debugf("err_code = %lx\n", regs->err_code);
    if(cr2 > 0)  debugf("cr2 = %lx\n", cr2);
    debugf("rbp = %lx\n", regs->rbp);
    debugf("rip = %lx\n", regs->rip);
    debugf("rsp = %lx\n", regs->rsp);
    debugf("cs = %lx\n", regs->cs);
    debugf("rflags = %lx\n", regs->rflags);
    debugf("cr3 = %lx\n", regs->cr3);
    debugf("ds = %lx\n", (uint64_t)(regs->ds & 0xffff));
    debugf("ss = %lx\n", regs->ss);
    debugf("rax = %lx\n", regs->rax);
    debugf("rbx = %lx\n", regs->rbx);
    debugf("rcx = %lx\n", regs->rcx);
    debugf("rdx = %lx\n", regs->rdx);
    debugf("rdi = %lx\n", regs->rdi);
    debugf("rsi = %lx\n", regs->rsi);
    debugf("r8 = %lx\n", regs->r8);
    debugf("r9 = %lx\n", regs->r9);
    if(!extended) return;
    debugf("r10 = %lx\n", regs->r10);
    debugf("r11 = %lx\n", regs->r11);
    debugf("r12 = %lx\n", regs->r12);
    debugf("r13 = %lx\n", regs->r13);
    debugf("r14 = %lx\n", regs->r14);
    debugf("r15 = %lx\n", regs->r15);
}


int last_num = -1;

void handle_error_generic(registers_t* regs, bool recover){

    int num = regs->int_no; int err = regs->err_code; //lazy fuck

    debugf("=======");
    if( 0 <= num && num < 32){
        debugf("%s = %i", exception_names[num], num);
        if(err){
            debugf("%s = %i", "Non-Zero Error Code", 0);
        }
        print_isr_regs(regs, (num == 1 || num == 3) ? 1 : 0);
    }
    else{
         debugf("%s = %i", "ISR_Unknown", num);
    }
    debugf("=======");

    if(num == last_num || !recover)
        panic("Can't Recover From Exception");
    
    last_num = num;

}


int num_pfs = 0;
void handle_pagefault(registers_t* regs){

    //intel manual 3141 4.12
    uint64_t cr2 = get_cr2();
   // __asm__ volatile ("movq %0, cr2" : "=r" (cr2) );
    debugf("\n== PAGE FAULT ==");
    debugf("Error: %lu \n", regs->err_code);
    debugf("At Address [CR2]: %lx \n", cr2);
    debugf("RIP = %lx \n", regs->rip);
    debugf(  (regs->err_code & 0b100) ? "USER" : "KERNEL");
    debugf(  (regs->err_code & 0b10) ? "WRITE" : "READ");
    debugf(  (regs->err_code & 0b01) ? "PAGE-PROTECTION VIOLATION" : "PAGE NOT PRESENT");

    if(regs->err_code & 0b1000)    debugf("CAUSE = RESERVED WRITE");
    if(regs->err_code & 0b10000)   debugf("CAUSE = INSTRUCTION FETCH");
    if(regs->err_code & 0b100000)  debugf("CAUSE = PROTECTION KEY VIOLATION");
    if(regs->err_code & 0b1000000) debugf("CAUSE = SHADOW STACK ACCESS");

    debugf("==============");


    if(num_pfs > 3){
        print_isr_regs(regs, 1);
        panic("Triple Fault!");
    }
    num_pfs++;

  //  if((regs->err_code & 0b1000) ) return;
    
   
    
        
   
}


int num_gpfs  = 0;
void handle_gpf(registers_t* regs){
    num_gpfs++;
    debugf("\n=== GPF ===");
    debugf("Error: %lu \n", regs->err_code);
    debugf("rbp %lx \n", regs->rbp);
    debugf("==============");
    //debugf("rsp %lx \n", regs->esp);
    print_isr_regs(regs, 1);



    if(num_gpfs > 1)
        panic("Triple Fault!");
        

}

void isr_handler(uint64_t rdi, registers_t regs)
{ // really should get a pointer to stack where regs struct is instead of pass by ref
#ifdef DEBUG
     print_isr_regs(&regs, 1); //we are frustrated and want these ASAP
#endif
    bool recoverable = 1;
  
    if(regs.int_no == ISR_InvalidOpcode) recoverable = 0;
    switch (regs.int_no)
    {
    case ISR_PageFault:
        handle_pagefault(&regs);
        break;
    case ISR_GeneralProtectionFault: //sorry for u bro
        handle_gpf(&regs);
        break;
    default: 
        handle_error_generic(&regs, recoverable); break;

    }
}

void irq_handler(uint64_t rdi, registers_t regs)
{
#ifdef DEBUG
    irq_log(regs.int_no, regs.err_code);
#endif
    // PIC_sendEOI(regs.int_no); //dunno
    if (interupt_handlers[(uint8_t)(regs.int_no & 0xff)] != 0)
    {
        
        isr_t handler = interupt_handlers[regs.int_no];
        handler(&regs);
    }
    lapic_sendEOI();
   
}

void register_interupt_handler(uint8_t n, isr_t handler_fn)
{
    interupt_handlers[n] = handler_fn;

    debugf("registered interupt handler for [%i] at %lx\n", n, (uintptr_t)handler_fn);
 

}
