#pragma once
#include "../exception/panic.h"


#define ASSERT(condition) if( !( condition ) ) bad_assert(__FILE__, __LINE__) 



static inline bad_assert(const char* file, int line){
        panic_info("Assertion Failed!", file, line);
}