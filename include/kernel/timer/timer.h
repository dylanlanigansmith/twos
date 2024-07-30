#pragma once

#include <kstdlib.h>

typedef uint64_t femtosec_t; 

typedef uint64_t microsec_t; 

typedef uint64_t millisec_t; 


typedef uint64_t timer_tick_t; 

#define ms_to_us(millis) (microsec_t)(millis * 1000ull)

#define s_to_us(seconds) (microsec_t)(seconds * 1000ull * 1000ull)