
C_SOURCES = $(wildcard kernel/*.c kernel/**/*.c drivers/*.c drivers/**/*.c)

CPP_SOURCES = $(wildcard kernel/*.cpp kernel/**/*.cpp drivers/*.cpp drivers/**/*.cpp)
HEADERS = $(wildcard kernel/*.h kernel/**/*.h drivers/**/*.h)
CPP_HEADERS = $(wildcard kernel/*.hpp kernel/**/*.hpp drivers/**/*.hpp)

ASM =  $(wildcard kernel/asm/**.asm kernel/boot/*.asm kernel/cpp/*.asm)
#should Make sources dep on all header files
OBJ = ${C_SOURCES:.c=.o}
CPP_OBJ = ${CPP_SOURCES:.cpp=.o}
ASM_OBJ = ${ASM:.asm=.o}

CRTI_OBJ=kernel/cpp/crti.o
CRTBEGIN_OBJ:=$(shell g++ -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell g++ -print-file-name=crtend.o)
CRTN_OBJ=kernel/cpp/crtn.o



C_FLAGS=-ffreestanding -fno-pie -fno-stack-protector -mno-shstk -fmacro-prefix-map=/home/dylan/code/randos=.
CPP_FLAGS=-nostdlib -nostartfiles -fno-exceptions -fno-rtti


OBJ_LINK_LIST:= $(OBJ) $(CPP_OBJ) $(ASM_OBJ) #$(CRTI_OBJ) $(CRTBEGIN_OBJ) $(CRTEND_OBJ) $(CRTN_OBJ)

all: iso

run: clean all
	qemu-system-x86_64 -cdrom os.iso -device VGA,vgamem_mb=32 -audiodev pa,id=speaker -machine pcspk-audiodev=speaker -serial file:com1.log -m 8G
rund: clean all
	qemu-system-x86_64 -s -S -cdrom os.iso -device VGA,vgamem_mb=32 -audiodev pa,id=speaker -machine pcspk-audiodev=speaker -serial file:com1.log
clean:
	rm -rf *.bin *.dis *.o *.iso
	rm -rf kernel/*.o boot/*.bin drivers/*.o
	rm -rf iso/kernel.bin
	find . -type f -name '*.o' -delete

iso: kernel.bin
	cp kernel.bin iso/boot/
	grub-mkrescue -o os.iso iso 

kernel.bin: $(OBJ_LINK_LIST)
	ld --nmagic -no-pie  -o $@ --script=linker.ld $^

%.o : %.asm
	@echo "Compiling ASM$<"
	nasm $< -f elf64 -o $@

#%.o : ${ASM}
#	nasm $< -f elf64 -o $@

%.o : %.c $(HEADERS)
	 @echo "Compiling C $<"
	gcc -masm=intel $(C_FLAGS)  -c $< -o $@

%.o : %.cpp $(HEADERS) $(CPP_HEADERS)
	@echo "Compiling CPP $<"
	g++ -masm=intel $(C_FLAGS) $(CPP_FLAGS) -c $< -o $@




#objdump -D -Mintel,i38 