#pragma once

#include "../stdlib.h"

/*
this whole file is fucked just use bitmasks

*/

#define BITMASK_21_ALIGN 0xFFFFFFFFFFE00000ULL

static inline uintptr_t align_to_16(uintptr_t address) {
    return address & 0xFFFFFFFFFFFFFFF0ull; // 16 LSBs == 0
}

static inline bool is_aligned_to_16(uintptr_t address) {
    return align_to_16(address) == address;
}


static inline uintptr_t align_to_bits(uintptr_t address, uint8_t alignment) { //wrong
    ASSERT (alignment != 0 && alignment < 64); 
    uintptr_t alignment_mask =  (1ULL << alignment) - 1;
    alignment_mask = ~alignment_mask;
    debugf("align_to(addr = %lx alignment = %i): generated mask = %lx", address, alignment, alignment_mask);
    return address & (alignment_mask - 1);
   //return address & (~((1ULL << (alignment - 1)) - 1));
}
static inline bool is_power_of_2(size_t n) {
    return n > 0 && ( (n & (n - 1)) == 0 );
}

static inline uintptr_t align_up(uintptr_t address, size_t alignment) {
    ASSERT (alignment != 0 && is_power_of_2(alignment)); 
   
   //if address < alignment  return 0
   if(address < alignment) return 0x0llu;
   
    return (address + (alignment - 1) ) & ~(alignment - 1);
   //return address & (~((1ULL << (alignment - 1)) - 1));
}
static inline uintptr_t align_to(uintptr_t address, size_t alignment) { //aligns down
    ASSERT (alignment != 0 && is_power_of_2(alignment)); 

     if(address < alignment) return 0x0llu;
   size_t remainder = address % alignment;

    // Subtract the remainder from the address to align it downwards
    return remainder;
  
  //  return address  & ~(alignment - 1);
  
}
static inline bool is_aligned_to(uintptr_t address, size_t alignment) {
    return align_to(address, alignment) == address;
}


static inline uintptr_t page_align(uintptr_t address) {
    return address & BITMASK_21_ALIGN ;
}

static inline bool is_page_aligned(uint64_t address) {
    return page_align(address) == address; //21 lsb == 0
}


static inline bool align_test_all(){
    const int test_cases = 4;
        uintptr_t testalign_16_valid[] = {
        0x0000000000000000,
        0x0000000000000010,
        0x0000000000000100,
        0x0000000000001000
    };

    uintptr_t testalign_16_invalid[] = {
        0x0000000000000001,
        0x000000000000000F,
        0x0000000000000101,
        0x0000000000000FFF
    };

    uintptr_t testalign_21_valid[] = {
        0x0000000000000000,
        0x0000000010000000,
        0x0000000020000000,
        0x0000000030000000
    };

    uintptr_t testalign_21_invalid[] = {
        0x0000000000000001,
        0x000000000FFFFF00,
        0x0000000010000001,
        0x000000002FFFFFFF
    };

    uintptr_t testalign_random_valid[] = {
        0x0000000000000000,
        0x0000000000800000,
        0x0000000001000000,
        0x0000000001800000
    };

    uintptr_t testalign_random_invalid[] = {
        0x0000000000000001,
        0x00000000007FFFFF,
        0x0000000000FFFFFF,
        0x00000000017FFFFF
    };

    for(int i = 0; i < test_cases; ++i){
         debugf("test alignment functions %i", i);
        ASSERT( is_aligned_to_16(testalign_16_valid[i]));
        ASSERT( !is_aligned_to_16(testalign_16_invalid[i]));
        ASSERT( is_page_aligned(testalign_21_valid[i]) );
        ASSERT( !is_page_aligned(testalign_21_invalid[i]) );
          debugf("page_align  = %lx  vs  align_to %lx  // input == %lx", page_align(testalign_21_invalid[i]), align_to(testalign_21_invalid[i], 0x200000), testalign_21_invalid[i]);
        ASSERT( page_align(testalign_21_invalid[i]) == align_to(testalign_21_invalid[i], 0x200000) );

        debugf("align 16  = %lx  vs  align_to %lx  // input == %lx", align_to_16(testalign_random_invalid[i]), align_to(testalign_random_invalid[i], 16), testalign_random_invalid[i] );
        ASSERT( align_to_16(testalign_random_invalid[i]) == align_to(testalign_random_invalid[i], 16) );

        ASSERT(is_page_aligned(page_align( testalign_random_invalid[i]  )) );
        ASSERT(is_aligned_to( page_align( testalign_random_invalid[i]  ),0x200000) );
    }
    /*
    notes:

    use premade masks for important stuff

  

    
    
    */
}