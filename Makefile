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
#$(info path = $(PATH))
#macs for whatever fucking reason dont let us just have a temp path within makefile soooo

TC_PREFIX:=$(CURDIR)/$(TOOLCHAIN_PATH)/bin/smith-

CC:=$(TC_PREFIX)gcc
CXX:=$(TC_PREFIX)g++
LD:=$(TC_PREFIX)ld
AS:=nasm







#==== SOURCES AND COMPILING=====
#https://stackoverflow.com/questions/2483182/recursive-wildcards-in-gnu-make/18258352#18258352
rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

#C_SOURCES = $(wildcard src/kernel/**.c)
C_SOURCES = $(call rwildcard,src,*.c)
ASM_SOURCES =  $(wildcard src/kernel/asm/*.asm src/boot/*.asm )

HEADERS = $(call rwildcard,include,*.h)


OBJ = ${C_SOURCES:.c=.o}

ASM_OBJ = ${ASM_SOURCES:.asm=.o}





C_FLAGS=-masm=intel -std=c2x -m64 -mcmodel=large -ffreestanding -nostdlib -fno-pie -fno-stack-protector -mno-shstk -mno-red-zone -fmacro-prefix-map=$(CURDIR)=.

C_FLAGS+= -I./include

#Linking
LD_FLAGS:=--nmagic --script=./src/linker.ld 

OBJ_LINK_LIST:= $(ASM_OBJ) $(OBJ) $(CPP_OBJ) 
#$
#
#
#for cpp but doesnt work probably need to build a cross compiler
#OBJ_LINK_LIST:= $(CRTI_OBJ) $(CRTBEGIN_OBJ) $(OBJ) $(CPP_OBJ) $(ASM_OBJ) $(CRTEND_OBJ) $(CRTN_OBJ) 


#====FILE STUFF====
ISO_OUT:=twos.iso
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
QEMU_ARGS_DBG:=-serial file:com1.log  -monitor telnet:127.0.0.1:6969,server,nowait;
#-d int,page,cpu_reset -s -S
QEMU_ARGS_DBG2:=-serial file:com1.log  -no-reboot -d int,page,cpu_reset -s -S  -monitor telnet:127.0.0.1:6969,server,nowait;
#,cpu_reset


QEMU_BASE_CMD:=$(VM)  -cdrom $(ISO_OUT) $(QEMU_ARGS_VM)

#			localhost tries ipv6 first 
QEMU_CONNECT_CMD:=telnet 127.0.0.1 6969

#====TARGETS======

all: iso

.PHONY: qterm
qterm:
	telnet localhost 6969 


.PHONY: runbg
runbg:
	@echo "== Starting QEMU =="
	screen -d -m $(QEMU_BASE_CMD) $(QEMU_ARGS_DBG) 
	@echo connecting...
	sleep 1
	$(QEMU_CONNECT_CMD)

.PHONY: rund
rund: clean all 
	@echo "======="
	$(QEMU_BASE_CMD) $(QEMU_ARGS_DBG2) 

.PHONY: run
run: clean all 
	@echo "======="
	$(QEMU_BASE_CMD) $(QEMU_ARGS_DBG) 

.PHONY: run_nor
run_nor: clean all
	@echo "Running With No Reboot, sorry that it's come to this"
	$(QEMU_BASE_CMD) $(QEMU_ARGS_DBG2) 


urun: clean all
	@echo "==Running in UEFI==="
	@echo "Using BIOS File $(QEMU_UEFI)"
	$(QEMU_BASE_CMD) -bios $(QEMU_UEFI) $(QEMU_ARGS_DBG) 

runb: clean all
#todo make this build with bochs flags for e9 hack or somethin
	@echo "==Running BOCHS==="
	bochs


iso: kernel.bin
	@cp kernel.bin $(ISO_ROOTDIR)/boot/
	@$(BOOT_CREATE) -o $(ISO_OUT) $(ISO_ROOTDIR) -quiet 
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
	@rm -rf *.bin *.dis *.o *.iso
	@rm -rf kernel/*.o boot/*.bin 
	@find . -type f -name '*.o' -delete
#like uh it could be really bad if iso_rootdir wasnt set and this ran as root but like who would do that 
	@rm -rf $(ISO_ROOTDIR)/boot/kernel.bin
	

cleanobj:
	find . -type f -name '*.o' -delete

