#pragma once
#include "kshared.h"

size_t strlen(const char* str);
size_t strnlen(const char *str, size_t max);

char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);

char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);

int strncmp(const char* str1, const char* str2, size_t num);
int strcmp(const char* str1, const char* str2);

char* itoa(int a, char* dest, int base);
char* ltoa(uint64_t a, char* dest, int base);

uint32_t atou(const char* str);