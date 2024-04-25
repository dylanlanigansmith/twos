### scratch pad for in progress stuff, not a write up or real notes, see README/SOURCES/BUILDING .md for formalities


``` 
ReadMe: https://pubs.opengroup.org/onlinepubs/9699919799/
POSIX standard ^
//post-mortem thoughts: ^ lmao yeah sure


things to fix now
- once this thing runs doom on my real modern hardware im never touching it again
- learned so much that it all must be rewritten!



plan:
1. VFS  [x]
2. MMAP from VFS [x]
3. Scheduler improvements [x]
4. Syscalls [x]
5. Doom [x]
6. Doom on real modern UEFI (thwarted by ps/2 emulation and difficult debugging)



- APIC probably should happen too but it looks like a whole thing
    - can config in 8529 legacy mode for uefi support.. [x] //we ended up doing this since it worked on our physical modern HW 
       


- refactor when we add syscalls
    - take all this printing BULLSHIT out of kernel 
    //by the time we got this far it was just about finishing the end goal of porting some software, pretty obvious where the flaws in current impl. are

- terminal [x]
- write some programs for _our operating system [x]

    



general todos/issues:

-physical memory manager [x]
  

- uefi support [~] 
    - no keyboard on modern modern hw yet (z690 chipset no CSM)



-stdout impl sucks, should do streams obviously & merge w/ vfs properly

-bulletproof kprintf with framebuffer support ASAP for debugging on real hw.



stdlib:
- printf-esque fns lack support for things like %2b and %2.f!! 
- finish CRT so we can have C++ support and a nicer _init->main handler 


fun ideas:

port doom [x]

simple window server even if just graphical tmux sorta thing



```