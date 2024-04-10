#pragma once
#include "../exception/panic.h"

#define ASSERT(condition) bad_assert(cond, __FILE__, __LINE__)


static inline bad_assert(int cond, const char* file, int line){
    if(!cond)
        panic_info("Assertion Failed!", file, line);
}