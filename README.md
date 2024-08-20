### twOS (two OS)

I write another operating system from scratch, because this is fun right?

So far it is going well. Memory management all setup, ready for porting my first OS to continue! 


**notes to self:

- memory management before anything else **mostly done**
    - pmm [**done**]
    - vmm 
        - page fault handler *todo* 
        - malloc [**done**]
        - map arbitrary page tables/pages [**done**]
        - vm_alloc [**done**]
    - kernel heap
        - port from randos [**done**]
        - fix buggy container [**done**]
        -  features to reimplement better:
            - resizing *todo - decide final kernel mmap* 
            - way to test aggresively *todo*
         
- interrupts are lazily ported
  
- filesystem rewrite

- better usermode gfx and posix file stuff

- dont write libc yourself this time

            
- streams and consoles/terminals
    - indenting even for serial
    - printf but someone else's better one
    


- pcie / usb 

- drivers as modules?



