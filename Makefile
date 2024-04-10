
C_SOURCES = $(wildcard kernel/*.c kernel/**/*.c drivers/*.c drivers/**/*.c)
HEADERS = $(wildcard kernel/*.h kernel/**/*.h drivers/**/*.h)

ASM =  $(wildcard kernel/asm/**.asm kernel/boot/*.asm)
#should Make sources dep on all header files
OBJ = ${C_SOURCES:.c=.o}
ASM_OBJ = ${ASM:.asm=.o}


all: iso

run: clean all
	qemu-system-x86_64 -cdrom os.iso -device VGA,vgamem_mb=32 -audiodev pa,id=speaker -machine pcspk-audiodev=speaker -serial file:com1.log -m 8G
rund: clean all
	qemu-system-x86_64 -s -S -cdrom os.iso -device VGA,vgamem_mb=32 -audiodev pa,id=speaker -machine pcspk-audiodev=speaker -serial file:com1.log
clean:
	rm -rf *.bin *.dis *.o *.iso
	rm -rf kernel/*.o boot/*.bin drivers/*.o
	rm -rf iso/kernel.bin

iso: kernel.bin
	cp kernel.bin iso/boot/
	grub-mkrescue -o os.iso iso

kernel.bin: ${OBJ} ${ASM_OBJ}
	ld --nmagic -no-pie -o $@ --script=linker.ld $^

%.o : %.asm
	nasm $< -f elf64 -o $@

#%.o : ${ASM}
#	nasm $< -f elf64 -o $@

%.o : %.c ${HEADERS}
	gcc -masm=intel -ffreestanding -fno-pie -fno-stack-protector -mno-shstk  -c $< -o $@

	





#objdump -D -Mintel,i38 