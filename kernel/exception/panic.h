#pragma once
#define KPANIC_QUICK(str) panic(str)
#define KPANIC_EARLY() panic("early") //so this will be what we call if we dont have printf yet or something NOT IMPL SHOULD BE
#define KPANIC(str) panic_info(str, __FILE__, __LINE__)

void panic(const char* str);

void panic_info(const char *str, const char* file, int line);