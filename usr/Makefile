USE_TOOLCHAIN:=1

PARENT_DIR := $(abspath $(dir $(CURDIR))/..)/randos


ifdef USE_TOOLCHAIN
#	@echo using toolchain
	TOOLCHAIN_PATH:=toolchain/cross
#	@echo adding to path $(CURDIR)/$(TOOLCHAIN_PATH)/bin:$(PATH)
	export PATH := $(PARENT_DIR)/$(TOOLCHAIN_PATH)/bin:$(PATH)

	TC_PREFIX:=smith-

	CC:=$(TC_PREFIX)gcc
	CXX:=$(TC_PREFIX)g++
	LD:=$(TC_PREFIX)ld
	AS:=nasm
#nasm
#as -msyntax=intel -mnaked-reg
else
	CC:=gcc
	CXX:=g++
	LD:=ld
	AS:=nasm

endif

C_SOURCES = $(wildcard **.c)

C_HEADERS = $(wildcard **.h)

C_FLAGS_COMMON:=-masm=intel -m64 -ffreestanding -fno-stack-protector -nostdlib

C_FLAGS:=$(C_FLAGS_COMMON) -no-pie 
LD_FLAGS:=$(C_FLAGS_COMMON) -no-pie 


LIB_NAME:=libd.a

OUT_FILE:=init


OBJS:= $(C_SOURCES:.c=.o)


all: init ls

init: libd $(OBJS) 
	$(CC) $(LD_FLAGS) -o  bin/init init.o -L$(CURDIR) -ld 

ls: libd $(OBJS) 
	$(CC) $(LD_FLAGS) -o  bin/ls ls.o -L$(CURDIR) -ld 


%.o : %.c $(C_HEADERS)
	@echo "[ $< ]"
	$(CC) $(C_FLAGS) -c $< -o $@


libd:
	$(MAKE) -C ../libd clean install
#
copy: clean all
	cp bin/* ../util/initrd/root/usr/bin/

install: copy
	$(MAKE) -C ../util/initrd install 
	

clean:
	rm -rf *.a
	rm -rf *.o
	rm -rf *.so
	rm -rf bin/*
	rm -rf *.rd
