
C_SOURCES = $(wildcard kernel/*.c kernel/**/*.c drivers/*.c drivers/**/*.c)
HEADERS = $(wildcard kernel/*.h kernel/**/*.h drivers/**/*.h)
#should Make sources dep on all header files
OBJ = ${C_SOURCES:.c=.o}


all: os-image

run: clean all
	qemu-system-i386 -drive file=os-image,format=raw,index=0,if=floppy -device VGA,xres=720,yres=400


os-image: boot/boot_sect.bin kernel.bin
	cat $^ > os-image

kernel.bin: kernel/kernel_entry.o ${OBJ}
	ld -m elf_i386 -o $@ -Ttext 0x1000 $^ --oformat binary
#-m elf_i386 for 32 bit

%.o : %.c ${HEADERS}
	gcc -m32 -masm=intel -ffreestanding -fno-pie -c $< -o $@

%.o : %.asm
	nasm $< -f elf -o $@

%.bin : %.asm
	nasm $< -f bin -I './boot/' -o $@

clean:
	rm -rf *.bin *.dis *.o os-image
	rm -rf kernel/*.o boot/*.bin drivers/*.o



#objdump -D -Mintel,i38 