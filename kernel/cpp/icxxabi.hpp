#pragma once

//https://wiki.osdev.org/C%2B%2B
#define ATEXIT_MAX_FUNCS	128

extern "C"
{
    typedef unsigned uarch_t;
    int __cxa_atexit(void (*f)(void *), void *objptr, void *dso);
void __cxa_finalize(void *f);

void __cxa_pure_virtual();
}