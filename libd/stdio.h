#pragma once

#include "stdint.h"

#include <stdarg.h>
#define PRINTF_BUF 1024

 void print(const char* str);

void putc(char c);
static inline void putchar(char c){
   putc(c);
}



typedef void FILE;

#define stderr 0
#define stdout 0

static inline int fflush(FILE* f){
     return 0;
}

void printf(const char* fmt, ...) ;

void puts(const char*  str);

void vprintf(const char* fmt, va_list ap) ;
int snprintf(char *s, size_t n, const char* fmt, ...);

int fprintf(FILE *restrict f, const char *restrict fmt, ...);
int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap);

int vsnprintf(char *s, size_t n, const char * fmt, va_list args);
int sscanf(const char *str, const char *format, ...);

static inline FILE *fopen(const char *restrict filename, const char *restrict mode){ return 0;}
static inline int fclose(FILE* f){ return 0;}

static inline size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) { return 0; }
static inline size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) { return 0; }

static inline void* mkdir(void* l, int p) { return 0;}

static inline long int ftell(FILE *stream) { return 0; }
#define SEEK_SET 0

#define SEEK_CUR 1

#define SEEK_END 2
static inline int fseek(FILE *stream, long int offset, int whence) { return 0; }


static inline int remove ( const char * filename ) { return 0; }

static inline int rename ( const char * oldname, const char * newname ) { return 0; }


static inline int get_highest_bit(uint32_t num) { //index of msb
    if (num == 0) return 0; 
    int position = sizeof(num) * 8 - 1; 
    while (!(num & (1u << position))) {
        position--; 
    }
    return position;
}