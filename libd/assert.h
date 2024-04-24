#pragma once
#include "os.h"
#include "stdio.h"


#define ASSERT(condition) if( !( condition ) ) __on_fail_assert(#condition, __FILE__, __LINE__) 
#define assert(c) ASSERT(c)


static inline void __attribute__((noreturn)) __on_fail_assert(const char* cond, const char* file, int line){
    //todo:
    printf("\n!!Assertion (%s) Failed @ '%s':(%i)!!\n", cond, file, line);
    exit(EXIT_ASSERTION); 
} 