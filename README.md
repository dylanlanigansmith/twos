### twOS (two OS)

I write another operating system from scratch, because this is fun right?


Ideas:

- memory management before anything else
    - pmm [**done**]
    - vmm 
        - page fault handler *todo* 
        - malloc [**done**]
        - map arbitrary page tables/pages [**done**]
        - vm_alloc [**done**]
    - kernel heap
        - port from randos [**done**]
        - fix buggy container [**done**]
        - add resizing *todo* 
        - test aggresively *todo* 
            
- streams and consoles/terminals
    - indenting even for serial
    - printf but good
    
- vfs but actually fully POSIX compliant this time even though it is grindy

- pcie / usb 

- drivers as modules?


- don't write libc all yourself this time 
    - yes you can have a good printf
        - but think of all the good times you have had with the one you wrote oh so long ago (and the formats u added that u use all over)
        - "good times" as if it wasnt purely for convoluted print-statement debugging
        - mostly want char width (%4s %04i); 