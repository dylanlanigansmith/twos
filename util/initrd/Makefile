#for initrd tools

CC:=gcc
CXX:=g++


CPP_SOURCES = $(wildcard **.cpp)

CPP_HEADERS = $(wildcard **.hpp)

CPP_FLAGS:=-Og -std=c++20 

OUT_FILE:=make_initrd

TEST_ARGS:=./root/ init.rd

all: make_initrd

make_initrd:
	$(CXX) $(CPP_SOURCES) $(CPP_FLAGS) -o $(OUT_FILE)

run: clean all
	@./$(OUT_FILE) $(TEST_ARGS)

install: run
	@cp init.rd ../../iso/boot/init.rd

clean:
	rm -rf *.o
	rm -rf $(OUT_FILE)
	rm -rf *.rd

 