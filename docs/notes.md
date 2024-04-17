### scratch pad for in progress stuff, not a write up or real notes, see README/SOURCES/BUILDING .md for formalities


``` 
ReadMe: https://pubs.opengroup.org/onlinepubs/9699919799/
POSIX standard ^


things to fix now
- need physical memory management or at least a semblance of it


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
    - task end 
    - task yield
    - actually finish the base functionality lol
        (remove hardcoded in examples for testing)
4. Syscalls
5. Doom

notes: along the way maybe do PMM !
APIC probably should happen too but it looks like a whole thing
    - can config in 8529 legacy mode for uefi support..
        - damn whyd you have to tempt me like that intel i see a shortcut i take IT
    https://wiki.osdev.org/APIC


- refactor when we add syscalls
    - take all this printing BULLSHIT out of kernel
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
-stdout sucks and is evil and is bad and it sucks and its evil and its bad and it sucks 
    - do you get the point?

STDOUT:

- if we arent sure it is up and running then:
- we should really just gfx_print (esp for interupts)
    - basically the idea of SAFE PRINT



stdlib:
https://pubs.opengroup.org/onlinepubs/9699919799/
printf binary needs like %10b to only print 10 bits etc 




- should make standardized more
    - decide if we wanna use gcc stdint etc (sigh prob should)


fun ideas:

port assembler or write tiny one

port doom
->see github portable doom repo looks easy

simple window server even if just graphical tmux sorta thing



```