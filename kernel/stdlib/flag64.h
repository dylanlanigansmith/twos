#pragma once
#include "stdint.h"
#include "stdbool.h"

//never gonna actually use this should chop it

typedef uint64_t* flag_t; 

inline static void set_flag(flag_t var, flag_t flag ){
    *var |= *flag;
}
inline static void clear_flag(flag_t var, flag_t flag){
     *var &= *flag;
}

inline static bool has_flag(flag_t var, flag_t flag){
    return (*var & *flag);

}