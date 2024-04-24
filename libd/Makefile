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

C_FLAGS:=-masm=intel -m64 -ffreestanding -fno-stack-protector -nostdlib -no-pie 

OBJS := $(C_SOURCES:.c=.o)

# Library name
LIB := libd.a

$(info Sources: $(C_SOURCES))
$(info Headers: $(C_HEADERS))
$(info Objects: $(OBJS))

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $(LIB) $(OBJS)

%.o : %.c $(C_HEADERS)
	@echo "[ $< ]"
	$(CC) $(C_FLAGS) -c $< -o $@


install: clean all
	cp 	 $(LIB) ../usr/$(LIB)
	@echo Copied $(LIB) to ../usr/$(LIB)
	cp 	 $(LIB) ../usr/port/doom-myos/doomgeneric/$(LIB)
	@echo Copied $(LIB) to ../usr/port/doom-myos/doomgeneric/$(LIB)


clean:
	rm -rf *.o 
	rm -rf *.a

