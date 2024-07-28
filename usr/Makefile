USE_TOOLCHAIN:=1

PARENT_DIR := $(abspath $(dir $(CURDIR))/..)/randos
CRTDIR:=$(PARENT_DIR)/crt

CRTI_OBJ:=$(CRTDIR)/crti.o
CRT0_OBJ:=$(CRTDIR)/crt0.o
CRTN_OBJ:=$(CRTDIR)/crtn.o
CRT_START:=$(CRTI_OBJ) $(CRT0_OBJ)
CRT_END:=$(CRTN_OBJ) 
CRT_OBJS:=$(CRT_START) $(CRT_END)


CRT_ASM = $(wildcard $(CRTDIR)/**.asm)



TOOLCHAIN_PATH:=toolchain/cross
export PATH := $(PARENT_DIR)/$(TOOLCHAIN_PATH)/bin:$(PATH)

TC_PREFIX:=$(PARENT_DIR)/$(TOOLCHAIN_PATH)/bin/smith-

CC:=$(TC_PREFIX)gcc
CXX:=$(TC_PREFIX)g++
LD:=$(TC_PREFIX)ld
AS:=nasm




$(info $(CRT_ASM) )

C_SOURCES = $(wildcard **.c)

C_HEADERS = $(wildcard **.h)



C_FLAGS:=-masm=intel -m64 -ffreestanding -fno-stack-protector -nostdlib -no-pie
LD_FLAGS:=-nostdlib 

LIB_NAME:=libd.a

OUT_FILE:=init


C_OBJS:= $(C_SOURCES:.c=.o)



OBJS = $(C_OBJS) $(CRT_OBJS)

OBJS_ALL:= $()


all: init ls



	

init: libd  $(OBJS) 
#	$(CC) $(LD_FLAGS) -o  bin/init $(CRT_START)  init.o $(CRT_END)  -L$(CURDIR) -ld 
	$(LD) $(LD_FLAGS) $(CRT_START)  init.o $(CRT_END) -L$(CURDIR) -ld   -o  bin/$(OUT_FILE)

ls: libd $(OBJS) 
	$(LD) $(LD_FLAGS) $(CRT_START)  ls.o $(CRT_END) -L$(CURDIR) -ld   -o  bin/ls

%.o : %.asm
	$(AS) $< -f elf64 -o $@

%.o : %.c $(C_HEADERS)
	@echo "[ $< ]"
	$(CC) $(C_FLAGS) -c $< -o $@


libd:
	$(MAKE) -C ../libd install
#
copy: clean all
	cp bin/* ../util/initrd/root/usr/bin/

install: copy
	$(MAKE) -C ../util/initrd install 
	
crt: clean_crt all
	@echo Making CRT
	

clean_crt:
	@echo Cleaning CRT
#	rm -rf $(CRTDIR)/*.o

clean: 
	rm -rf *.a
	rm -rf *.o
	rm -rf *.so
	rm -rf bin/*
	rm -rf *.rd
