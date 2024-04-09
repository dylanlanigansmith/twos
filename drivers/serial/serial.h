#pragma once
#include "../../kernel/stdlib.h"
#define SERIAL_PORT_COM1 0x3f8

int serial_init();

void serial_write(char a);

void serial_print(const char* str);