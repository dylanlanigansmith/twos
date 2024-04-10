#pragma once
#include <stdarg.h> //we can have this?!
#include "stdint.h"
//todo: standardize this to work with serial and gfx
#define KPRINT(str) print(str)
#define KDEBUG(str) print(str)

#define PRINT_SERIAL
#define PRINT_GRAPHICAL

void print(const char* str);
void println(const char* str);

void printh(uint64_t hex);
void printi(int i);
void printl(uint64_t ll, int base);


#define PRINTF_MAX 1024
void printf(const char* fmt, ...) ;
size_t sprintf(char* buf, size_t len, const char* fmt, ...);





//__vprintf should take the print function as an arg tbh so we canuse it for other shit
 //this also doesnt need to be public unless ^
size_t __vprintf(const char *fmt, char *buf_out, size_t len, va_list args); //this one actually works its what i get for not just writing my own to start with