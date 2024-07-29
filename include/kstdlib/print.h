#pragma once
#include "string.h"

#define PRINTF_BUF 2048

int snprintf ( char * s, size_t n, const char * format, ... );
int vsnprintf (char * s, size_t n, const char * format, va_list arg );


