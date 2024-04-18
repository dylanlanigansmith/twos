CC:=gcc
CXX:=g++


C_SOURCES = $(wildcard **.c)

C_HEADERS = $(wildcard **.h)

C_FLAGS:=-masm=intel -m64 -ffreestanding -no-pie -fno-stack-protector -nostdlib

OUT_FILE:=usermode



all: usermode

usermode:
	$(CC) $(C_SOURCES) $(C_FLAGS) -o $(OUT_FILE)


clean:
	rm -rf *.o
	rm -rf $(OUT_FILE)
	rm -rf *.rd
