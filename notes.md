### scratch pad for in progress stuff, not a write up or real notes, see README/SOURCES/BUILDING .md for formalities


``` 
 
things to fix now
- need physical memory management or at least a semblance of it

MAKE TASKING BETTER
    -HARD BC I CANT THINK OF ANY TASKS
    -add task end
    -add task yield

file system sorta thing


FIGURE OUT HOW TO JUMP TO USER MODE
then we can jump 2 user mode
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





fun ideas:

port assembler or write tiny one

port doom
->see github portable doom repo looks easy

simple window server even if just graphical tmux sorta thing



```