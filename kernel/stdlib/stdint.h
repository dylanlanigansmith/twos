#pragma once

#define nullptr 0
#ifdef __cplusplus
extern "C" {
#endif
typedef unsigned char uint8_t;
typedef char int8_t;


typedef unsigned short uint16_t;
typedef short int16_t;

typedef unsigned int uint32_t;
typedef int int32_t;

//typedef unsigned int uintptr_t; //32 bit


typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef uint64_t uintptr_t;

#ifndef __cplusplus
typedef unsigned long long  size_t; 
#else
typedef unsigned long  size_t; 
#endif

#ifdef __cplusplus
}
#endif