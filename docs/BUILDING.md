### Dependencies and Build Env


- GRUB2 based host system to use Makefile

        > grub-mkrescue --version 
            (GRUB) 2:2.12-2

        # note that grub mkrescue depends on xoriso and some gnu util packages that may not be installed via a distro's default grub package
        # if it gives errors this is likely the cause
- Compilers:
        >gcc / buildutils toolchain
         just run install_toolchain.sh in toolchain/ it's fairly self explanatory

- QEMU
        > qemu-system-x86_64 --version
        QEMU emulator version 8.2.2
        (also used bochs and virtualbox for their respective strengths, bochs is best for descriptor table issues etc, and if it boots in VB you have a real shot at physical HW)
        
        
### Building

##TODO: Update for toolchain


Assuming you are on a system with gcc, nasm, and grub-mkrescue all ready:

    git clone [this repo]
    cd randos
    make configure # this checks for compilers , etc, nothing fancy 
    make clean all

## Testing 

    make run //run kernel as is
    make usr run //build usr programs, copy usr/bin to initrd root, generate initrd, then build kernel
    //replace 'run' w/ 'urun' to test in UEFI (pivoting towards that as the primary platform now that it boots on real HW)

Serial debug output will be sent to file specified in QEMU args, if the kernel is passed appropriate parameters via the bootloader 




### Notes: 
- I use grub-mkrescue to use the currently installed GRUB version (GRUB) 2:2.12-2 on my arch host system to create the bootable ISO
    - GRUB is licensed under GPL so I have been careful not to have any actual GRUB code in this repo
    - building on non-GRUB using systems may be tricky so if I ever work on this from my mac I will have to package GRUB with it
    - this is mostly a reminder to myself not to break GPL by accident or Richard Stallman will show up at my door and he won't be happy

- 
