### scratch pad for in progress stuff, not a write up or real notes, see README/SOURCES/BUILDING .md for formalities


``` 
 
things to fix now
- need physical memory management or at least a semblance of it

MAKE TASKING BETTER
    -HARD BC I CANT THINK OF ANY TASKS
    -add task end
    -add task yield

file system sorta thing

WE CAN JUMP TO USER MODE!
    - now what?
    - need to expand tasking
        - user tasks
            -to make a user task:
                -map a user address space
                -load user executable or whatever
                -call main
            - if a user task faults we kill it
        - kernel tasks
            - are we gonna have stdout run as a kernel task ?
            - we probs should move a lot of it to user space
    - streams / buffers will be useful
        - like for event queues etc and shit


    - need to add syscalls
        - interupts 

Should we figure out APIC before adding more IDT stuff ? 

Should we do a VFS for loading user programs ?

plan:



1. VFS 
2. MMAP from VFS
3. Scheduler improvements
4. Syscalls
5. Doom

notes: along the way maybe do PMM !


- refactor when we add syscalls
    - take all this SHIT out of kernel
- terminal
- write some programs for _our operating system_
-^ see prev.
    



general todos/issues:

-physical memory manager
    - parse mem map
    - remove hardcoded phys maps
    - should be able to run on 2gb at least, ideally only ID map first 128mb or something 

- uefi support
    - need apic, hpet 
    - multiboot2 uefi headers 

- physical hardware
    - uefi prereq. 
    - mmap parsing prereq.

STDOUT:

- if we arent sure it is up and running then:
- we should really just gfx_print (esp for interupts)
    - basically the idea of SAFE PRINT



stdlib:

printf binary needs like %10b to only print 10 bits etc 




- should make standardized more
    - decide if we wanna use gcc stdint etc (sigh prob should)


fun ideas:

port assembler or write tiny one

port doom
->see github portable doom repo looks easy

simple window server even if just graphical tmux sorta thing



```