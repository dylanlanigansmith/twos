# I Wrote An Operating System
### no like for real too, it isn't just booting and printing "hello world"
##### I'm pretty proud of this, it has been quite hard. It runs DOOM, not just in a VM either! 

TODO: THIS IS ALL OUT OF DATE MOST OF THE STUFF IS DONE AND WORKING
IT BOOTS ON REAL MODERN UEFI PCS AND RUNS DOOM, IN USER MODE, LOADED FROM A CUSTOM FILESYSTEM AS AN ELF64 EXECUTABLE BUILT WITH OUR CROSS COMPILER, WITH SUPPORT FOR MULTI TASKING ALL ALONG THE WAY
BADA BING BADA BOOM


## Feature Overview
    - x86 64 bit
    - Multiboot2 spec. compatible
    - VGA/VBE Graphics Drivers, full HD 1080p, 32 bit color support
    - Filesystem
    - Multi-tasking Kernel
    - The only thing that isn't from scratch is the use of GRUB/Multiboot bootloader and the base Doom src which took quite a few tweaks to port
        - Built completely freestanding, custom libc, from scratch standard library, etc.
        - Custom memory allocator/heap, custom scheduler
    - C and C++ Support, Custom Toolchain/Cross Compiler via GCC/Binutils
        - Automated Installation and setup! 
    - Kernel is written in primarily C and Assembly 
        - I keep saying I will use C++ after being a huge proponent of it for a long time, but then write more C.. wouldn't want to write a 3D engine in C, wouldn't want to write an OS in C++...
        - I don't think 

## In Progress
    - At this point all it could use is some input tweaks for playing DOOM and a USB Keyboard driver for real hardware (works with ps2! luckily my very new z690 mobo has a port suprisingly)
    
## Planned
    - double buffering (sorta)
    - Port DOOM (as a user mode process) [x]
    - Cross compiler + C Runtime for easier use of our standard library [x]
    - UEFI Support to boot on real hardware [x]
    - now that we have reached user mode a lot of kernel code can be tidied up and moved to userspace, or refactored into a API for usage from user space
        - you can tell what was added in the last stage of this project 
        - I would like to rewrite this someday, learned too many lessons not to


#### Detailed Features
- VBE Framebuffer, up to 32 bit color, Graphics Driver provides API 

- Serial/COM port support, kernel debugging via serial

- Kernel / CPU Exception interupt handlers with memory dumps and many features to make development easy

- Multi-tasking, round robin scheduler

- Fully implemented from scratch heap and malloc/free 
    - this has been one of the most fun things I have ever programmed and I have many ideas to improve its efficiency

- a stdout-type buffer and text renderer

- custom standard library, with strings, printf, malloc, memory functions, etc. all implemented from scratch
    - one for kernel and one for user space, user space has it's own C Runtime
    - libc-ish enough that stuff is portable
- it runs Doom, really well too. and in multi-tasked user space... no cheating!

- custom initrd filesystem and VFS implementation, with subdirs, etc. 

- A lot more, TODO







#### Notes/Misc
- writing asm is sorta zen

- this was my first big C project and first I was missing C++ a lot.. then things started coming along and TLDR: C and I are on good terms now 
    - when I finally added C++ support thinking I'd want to rewrite/continue with it, I was underwhelmed and went back to C LOL
    - I really see the appeal of a "C+" type language now, like C with QoL stuff (auto, function/operator overloads, basic constexpr, etc.), and "classes" that are just structs with functions, etc. without getting into C++ abstractions like streams and templates too much
        - it should keep C calling conventions too 
        - next project ?
    - honestly the only thing I consistently miss from C++ is function over-loading, and some of how C++ handles structs (aka lemme stick functions in there so im not prefixing every function like "structname_dothing(example_t* examplestruct)")
        - im pretty sure if I switched to like the newest C20 / C23 I would get some of these features (unknown if I would without libc / stdlib though)
        - ooh i also miss being able to declare variables inside logical expressions, ie. 'if (int err = get_error(); err > 0 ) { print(err); }' is nice to have 
       

If you want to test your understanding of how a modern computer works, write an OS from scratch. I have been humbled and learned so much about concepts I thought I had down pat. Also you'll never forget the x64 ABI and calling conventions after this too. 
