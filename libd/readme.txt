NOTES ON LIBD (our 'libc'):

this is not and will never claim to be POSIX compliant

regarding the functions that are obviously stubs or have like:
         ASSERT(1 == 0) // (1 == 0) because it stands out to eye more re: intention
    - these are here to make doom work without ripping out tonnes of the code base despite modifying or implementing OS specific equivs where needed
    - way they did WAD file loading in DOOM is awesome, based on classes, so we just implement a 'class' in the doom src 'w_file_dls.c'
    - don't need any other files etc 
    - our *printf* impls suck
        - need support for like %.2d etc..
    - btw if you see any weird chunks of the doom code where a loop using snprintf has been unwrapped... those aren't necessary anymore..
        // on physical hardware snprintf behaved completely random so just gave up and unwrapped the weird ones that triggered it, still working on the memory issues on real HW 

string functions suck

printf-likes suck

need to port kmalloc allocator to usr mode malloc

malloc gets initialized via our custom C Runtime stuff 

OS v2 will have streams and proper impl of a lot of this in comparison

at this point im just bootstrapping to having ported code (doom) and then trying to clean up the mess enough to boot on a modern machine
crazy how it works flawlessly in EVERY VM yet on real HW is so all over the place
i respect the people writing modern drivers and OS code so much - VM debug tools rock, but theyre useless in the end 
need a fricken z690 motherboard with a serial port lol

