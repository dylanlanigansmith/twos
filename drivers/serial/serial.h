#pragma once
#include "../../kernel/stdlib.h"

#include <stdarg.h>

#define SERIAL_PORT_COM1 0x3f8

int serial_init();

void serial_write(char a);

void serial_print(const char* str);
void serial_println(const char* str);
void serial_printi(const char* str, int64_t i);
void serial_printh(const char* str, int64_t i);


void serial_set_e9();