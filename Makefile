#dls: makefile for randOS

USE_TOOLCHAIN:=1

ifdef USE_TOOLCHAIN
	@echo using toolchain
	TOOLCHAIN_PATH:=toolchain/cross
	@echo adding to path $(CURDIR)/$(TOOLCHAIN_PATH)/bin:$(PATH)
	export PATH := $(CURDIR)/$(TOOLCHAIN_PATH)/bin:$(PATH)

	TC_PREFIX:=smith-

	CC:=$(TC_PREFIX)gcc
	CXX:=$(TC_PREFIX)g++
	LD:=$(TC_PREFIX)ld
	AS:=nasm
else
	@echo no toolchain
	CC:=gcc
	CXX:=g++
	LD:=ld
	AS:=nasm

endif

VM:=qemu-system-x86_64
BOOT_CREATE:=grub-mkrescue


#==== SOURCES AND COMPILING=====
C_SOURCES = $(wildcard kernel/*.c kernel/**/*.c drivers/*.c drivers/**/*.c)
CPP_SOURCES = $(wildcard kernel/*.cpp kernel/**/*.cpp drivers/*.cpp drivers/**/*.cpp)
ASM_SOURCES =  $(wildcard kernel/asm/**.asm kernel/boot/*.asm kernel/cpp/*.asm kernel/task/*.asm)

HEADERS = $(wildcard kernel/*.h kernel/**/*.h drivers/**/*.h)
CPP_HEADERS = $(wildcard kernel/*.hpp kernel/**/*.hpp drivers/**/*.hpp)

OBJ = ${C_SOURCES:.c=.o}
CPP_OBJ = ${CPP_SOURCES:.cpp=.o}
ASM_OBJ = ${ASM_SOURCES:.asm=.o}

#for c++ to work, and not very well might i add
CRTI_OBJ:=kernel/cpp/crti.o
CRTBEGIN_OBJ:=$(shell g++ -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell g++ -print-file-name=crtend.o)
CRTN_OBJ:=kernel/cpp/crtn.o



C_FLAGS:= -masm=intel -m64 -ffreestanding -fno-pie -fno-stack-protector -mno-shstk -fmacro-prefix-map=/home/dylan/code/randos=.
CPP_FLAGS:=-nostdlib -nostartfiles -fno-exceptions -fno-rtti

#Linking
LD_FLAGS:=--nmagic --script=linker.ld -no-pie 

OBJ_LINK_LIST:= $(OBJ) $(CPP_OBJ) $(ASM_OBJ) 
#for cpp but doesnt work probably need to build a cross compiler
#OBJ_LINK_LIST:= $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJ) $(CPP_OBJ) $(ASM_OBJ) $(CRTEND_OBJ) $(CRTN_OBJ) 


#====FILE STUFF====
ISO_OUT:=randos.iso
ISO_ROOTDIR:=iso

#===QEMU======
QEMU_UEFI:=/usr/share/ovmf/x64/OVMF.fd
QEMU_ARGS_VM:=-accel kvm -device VGA,vgamem_mb=32 -audiodev pa,id=speaker -machine pcspk-audiodev=speaker -m 8G
QEMU_ARGS_DBG:=-serial file:com1.log  -d int

#====TARGETS======

all: iso

run: clean all
	@echo "======="

#let it be known that if i run with -machine type=q35 my memory allocations for heap map actually work at expected address but then i have to fix everything for that, likely why shit dont run in nothin else rn tho 
#really frustrating but ill just stick with qemu and be HAPPY
	$(VM)  -cdrom $(ISO_OUT) $(QEMU_ARGS_VM) $(QEMU_ARGS_DBG) 
#add -s -S for dbg 

urun: clean all
	@echo "==Running in UEFI==="
	$(VM)  -cdrom $(ISO_OUT) $(QEMU_ARGS_VM) -bios $(QEMU_UEFI) $(QEMU_ARGS_DBG) 

runb: clean all
#todo make this build with bochs flags for e9 hack or somethin
	@echo "==Running BOCHS==="
	bochs

iso: kernel.bin
	cp kernel.bin $(ISO_ROOTDIR)/boot/
	$(BOOT_CREATE) -o $(ISO_OUT) $(ISO_ROOTDIR) -quiet 
	@echo "==Made ISO=="


# === compiling targets == 


kernel.bin: $(OBJ_LINK_LIST)
	$(LD) $(LD_FLAGS)  -o $@ $^

%.o : %.asm
#@echo "Compiling $<"
	$(AS) $< -f elf64 -o $@

%.o : %.c $(HEADERS)
	@echo "[ $< ]"
	$(CC) $(C_FLAGS)  -c $< -o $@

%.o : %.cpp $(HEADERS) $(CPP_HEADERS)
	@echo " CPP [ $< ]"
	$(CXX) $(C_FLAGS) $(CPP_FLAGS) -c $< -o $@








#==== Other Util Targets ====

.PHONY: configure
configure:
	@echo "Setting up buildenv and ensuring dependencies"
	@echo "Note: This is for NO TOOLCHAIN and is now DEPRECATED"
	@echo 
	@echo "----Check Build Tools----"
	@command -v $(CC) > /dev/null || (echo "Error: $(CC) is not installed."; exit 1)
	@echo "Found $(CC)"
	@command -v $(CXX) > /dev/null || (echo "Error: $(CXX) is not installed."; exit 1)
	@echo "Found $(CXX)"
	@command -v $(AS) > /dev/null || (echo "Error: $(AS) is not installed."; exit 1)
	@echo "Found $(AS)"
	@command -v $(LD) > /dev/null || (echo "Error: $(LD) is not installed."; exit 1)
	@echo "Found $(LD)"
	@echo 
	@echo "----Check Image Creation and VM----"
	@command -v $(BOOT_CREATE) > /dev/null || (echo "Error: $(BOOT_CREATE) is not installed."; exit 1)
	@echo "Found $(BOOT_CREATE)"
	@command -v $(VM) > /dev/null || (echo "Error: $(VM) is not installed."; exit 1)
	@echo "Found $(VM)"
	@echo 
	@echo "!!! Configured Successfully !!!"

.PHONY: clean
clean:
#rewrite this
	rm -rf *.bin *.dis *.o *.iso
	rm -rf kernel/*.o boot/*.bin drivers/*.o
#like uh it could be really bad if iso_rootdir wasnt set and this ran as root but like who would do that 
	rm -rf $(ISO_ROOTDIR)/boot/kernel.bin
	find . -type f -name '*.o' -delete

