#pragma once
#include <kstdlib.h>


enum KLOGLEVELS {
    LOG_DEFAULT = 0u,
    LOG_DEBUG = 0u,
    LOG_CRITICAL,
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO,
};

uint8_t printk(uint8_t level, const char* fmt, ...);


void _printk_vm(char* str);

#define debugf(...) printk(LOG_DEBUG, __VA_ARGS__); 
