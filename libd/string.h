#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

char* itoa(int val, int base);
char* lltoa(uint64_t val, int base);

char* htoa(uint64_t val); 

uint32_t atou(const char* str);
int atoi(const char* str);
char* strcpy(char* dest, const char* src);

char* strncpy(char* dest, const char* src, size_t num);
//
char* strncpy2(register char* t, register const char* f, size_t n);

char* strcat(char* dest, const char* src);
char* strncat(char* dest, const char* src, size_t n);
size_t strlen(const char* str);

size_t strnlen(const char* str, size_t num);

int strncmp(const char* str1, const char* str2, size_t num);

int strcmp(const char* str1, const char* str2);

char *strdup(const char *s); //actually want these
 char * strstr (const char * str1, const char * str2 ); //actually want these

int toupper(char c);
int tolower(char c);

int strcasecmp(const char *s1, const char *s2);

int strncasecmp(const char* s1, const char* s2, size_t n);

static inline const char * strchr ( const char * str, int character ) { return str; }

static inline const char* strrchr ( const char * str, int character ) { return (char*)(str + strlen(str) - 1); } //optimisitc

static inline int isspace(char c) { return (c==' ' || c=='\n' || c=='\t' || c=='\r' || c=='\f' || c == '\v'); }


static inline double atof(const char *s) { return 1.0; }


#ifdef __cplusplus
}
#endif