//iota shit can go here
#pragma once

#include "stdint.h"

char* itoa(int val, int base);
char* lltoa(uint64_t val, int base);

char* htoa(uint64_t val); 

char* strcpy(char* dest, const char* src);

char* strcat(char* dest, const char* src);

size_t strlen(const char* str);