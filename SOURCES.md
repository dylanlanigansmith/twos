## These are the resources I have used to create this OS so far:
###### (A bookmarks folder wasn't cutting it anymore)


- [Multiboot2 Spec via gnu.org](https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html)
    - I am glad that I wrote my own bootloader first before jumping to GRUB/Multiboot 
    - Awesome manual for what I thought would be a dreadful spec to implement
- [OSDev Wiki](https://wiki.osdev.org/)
    - Don't TRUST WHAT YOU READ HERE
    - But I can't deny it is as helpful as it is hindering
        - many an hour has been spent because of wrong specs stated here, or inconsistent articles that contradict eachother
- [Writing a Simple Operating System
from Scratch by Nick Blundell](https://www.cs.bham.ac.uk/%7Eexr/lectures/opsys/10_11/lectures/os-dev.pdf)
    - This project started by me working through this book, completing the assignments mentioned in it as if I was in the course it originated from
    - Of course, I got to the good part (post GDT protected mode), and realized the book was unfinished
    - Reaching the end of the textbook was like a cliffhanger of sorts, so I dove in on my own and now here we are!
    - None of the original code for the first version I made following the book is really used anymore
        - however, there is a seperate branch in the repo with my bootloader and a simple OS that expanded on this book before I switched to Multiboot2, 
            - it has interupts and VGA graphics and is a decent example for anyone who would like to use this book as a introduction
-  [Intel Developer Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
    - I spent the first stages of development mostly using wikis and textbooks, but once I realized how much OSDev content out there is just plain wrong, I went all in on RTFM 


- Tenacity
    - this has been hard but oh so rewarding 
    - the bugs you get are insanely frustrating, and the solutions are always insanely simple once you understand them
    - prev. point is why there is a lot of curse words in the comments, and some weird / messy areas in the code where insane debugging was done as I was second guessing everything that haven't been cleaned up yet
    - the way OSDev is difficult is interesting in comparison to some of my other large projects like game engines
    - I have gained so much respect for the Linux Kernel
        