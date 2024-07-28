#dls: makefile for randOS

UNAME_S := $(shell uname -s)

MACOS = 0
THIRDPARTY_BIN=
# Additional flags for macOS
ifeq ($(UNAME_S),Darwin)
    MACOS = 1
	THIRDPARTY_BIN = $(CURDIR)/thirdparty/build/bin/
endif
$(info building on Macos!)



TOOLCHAIN_PATH:=toolchain/cross
$(info adding to path $(CURDIR)/$(TOOLCHAIN_PATH)/bin:$$(PATH) )

export PATH := $(CURDIR)/$(TOOLCHAIN_PATH)/bin:$(PATH)
$(info path = $(PATH))
#macs for whatever fucking reason dont let us just have a temp path within makefile soooo

TC_PREFIX:=$(CURDIR)/$(TOOLCHAIN_PATH)/bin/smith-

CC:=$(TC_PREFIX)gcc
CXX:=$(TC_PREFIX)g++
LD:=$(TC_PREFIX)ld
AS:=nasm







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
#THIS IS BROKEN NOW
CRTI_OBJ:=kernel/cpp/crti.o
CRTBEGIN_OBJ:=$(shell $(CXX) -print-file-name=crtbegin.o)
CRTEND_OBJ:=$(shell $(CXX) -print-file-name=crtend.o)
CRTN_OBJ:=kernel/cpp/crtn.o



C_FLAGS:=-masm=intel -m64 -mcmodel=large -ffreestanding -nostdlib -fno-pie -fno-stack-protector -mno-shstk -mno-red-zone -fmacro-prefix-map=$(CURDIR)=.
CPP_FLAGS:=-nostartfiles -fno-exceptions -fno-rtti

#Linking
LD_FLAGS:=--nmagic --script=linker.ld 

OBJ_LINK_LIST:= $(ASM_OBJ) $(OBJ) $(CPP_OBJ) 
#$
#
#
#for cpp but doesnt work probably need to build a cross compiler
#OBJ_LINK_LIST:= $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJ) $(CPP_OBJ) $(ASM_OBJ) $(CRTEND_OBJ) $(CRTN_OBJ) 


#====FILE STUFF====
ISO_OUT:=randos.iso
ISO_ROOTDIR:=iso
BOOT_CREATE=grub-mkrescue
ifeq ($(MACOS),1)
    BOOT_CREATE:=$(THIRDPARTY_BIN)grub-mkrescue
endif
#===QEMU======

VM:=qemu-system-x86_64

QEMU_UEFI:=/usr/share/ovmf/x64/OVMF.fd

QEMU_ARGS_AUDIO=-audiodev pa,id=speaker
QEMU_ARGS_MEM=-m 8G
QEMU_ARGS_ACCEL=-accel kvm
ifeq ($(MACOS),1)
    QEMU_ARGS_AUDIO=-audiodev coreaudio,id=speaker
	QEMU_ARGS_MEM=-m 4G
	QEMU_ARGS_ACCEL=
endif
QEMU_ARGS_VM:=$(QEMU_ARGS_ACCEL) -device VGA,vgamem_mb=32 $(QEMU_ARGS_AUDIO) -machine pcspk-audiodev=speaker $(QEMU_ARGS_MEM)
QEMU_ARGS_DBG:=-serial file:com1.log  
#-d int,page,cpu_reset -s -S
QEMU_ARGS_DBG2:=-serial file:com1.log  -no-reboot -d int,page,cpu_reset -s -S
#,cpu_reset
#====TARGETS======

all: iso

run: clean all 
	@echo "======="
	$(VM)  -cdrom $(ISO_OUT) $(QEMU_ARGS_VM) $(QEMU_ARGS_DBG) 

rund: clean all
	@echo "Running With No Reboot, sorry that it's come to this"
	$(VM)  -cdrom $(ISO_OUT) $(QEMU_ARGS_VM) $(QEMU_ARGS_DBG2) 


urun: clean all
	@echo "==Running in UEFI==="
	$(VM)  -cdrom $(ISO_OUT) $(QEMU_ARGS_VM) -bios $(QEMU_UEFI) $(QEMU_ARGS_DBG) 

runb: clean all
#todo make this build with bochs flags for e9 hack or somethin
	@echo "==Running BOCHS==="
	bochs

runvb: clean all
	VBOX_GUI_DBG_AUTO_SHOW=1 VBOX_GUI_DBG_ENABLED=1 virtualbox 

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



# user / util targets
crt:
	@$(MAKE) -C usr crt

libd: crt
	@$(MAKE) -C libd clean all install

doom: libd
	@$(MAKE) -C usr/port/doom-myos/doomgeneric clean all install
#-j14
usr: doom
	@$(MAKE) -C usr clean all install




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
	

cleanobj:
	find . -type f -name '*.o' -delete


installreal:
	sudo cp iso/boot/kernel.bin /boot/
	sudo cp iso/boot/init.rd /boot/