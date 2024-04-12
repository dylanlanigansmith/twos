#pragma once
#include "stdint.h"

void* memcpy(void* dest, void* src, size_t num_bytes);


void* memset(void* dest, uint8_t val, size_t num_bytes);

void* memset_u32(volatile uint32_t* dest, const uint32_t val, const size_t num_ints); //THIS IS GONNA BE BUGGY 