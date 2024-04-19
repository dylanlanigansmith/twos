#pragma once

#include "../stdlib.h"
//surely i wont regret making up a phys mem manager system that NOONE HAS MENTIONED on OsDev wiki 
    //clearly it isnt flawed and they didnt think of it, right?
    //....
    //...

//typedef struct {  phy_entry_t entry[512]  } __attribute__((packed)) phy_t;
typedef struct {  uint64_t entry[8]  } __attribute__((packed)) phy2_t; //1024mb coverage
typedef struct {  phy2_t entry[256]  } __attribute__((packed)) phy3_t; //256gb coverage

//so we can add flags to level 2 and 3 for a semi buddy type deal

static_assert(sizeof(phy2_t) == 64, "");

typedef struct {  
    phy3_t entry[1]; //256gb coverage
    
} __attribute__((packed)) phy_table;



static_assert(sizeof(phy_table) < 32 * 1024, "");

typedef struct{
    uint8_t p4;
    uint16_t p3 ;
    uint32_t p2 ;
    uint32_t idx ;
    uint32_t bit ;
} __attribute__((packed)) phy_idx;
//static_assert(sizeof(phy_idx) == 4, ""); //removed bitfields as we are too early to be optimziing shit like that


static inline phy_idx get_indices_for_phyaddr(uintptr_t addr)
{
    uint32_t p2 = (addr >> 21) & 0x1ff;
    phy_idx ret = {
        .p4 = 0,
        .p3 = (addr >> 30) & 0x1ff,
        .p2 = p2,
        .idx = p2 / 64,
        .bit = p2 % 64
    };
  

    return ret;
    //p4 = 0, there only for expansion purposes 
}



static inline phy_idx get_indices_for_phyaddrd(uintptr_t addr)
{
    uint32_t p2 = (addr >> 21) & 0x1ff;
    phy_idx ret = {
        .p4 = 0,
        .p3 = (addr >> 30) & 0x1ff,
        .p2 = p2,
        .idx = p2 / 64,
        .bit = p2 % 64
    };
    debugf("for address %lx:\n", addr);
    debugf("p4 = %i p3 = %i p2 = %i \n", ret.p4, ret.p3, ret.p2);
    debugf("idx = %i bit = %i\n\n", ret.idx, ret.bit);
    return ret;
    //p4 = 0, there only for expansion purposes 
}


static inline uint8_t get_bit_at(phy2_t* p2, phy_idx* idx){
    return (p2->entry[idx->idx] >> idx->bit) & 1; 
}

static inline void set_bit_at(phy2_t* p2, phy_idx* idx){
    p2->entry[idx->idx] =  p2->entry[idx->idx] | (1ull << idx->bit);
}

static inline void clear_bit_atn(uint64_t* phy_idx,  uint8_t n){
    *phy_idx = *phy_idx & ~(1ull << n);
}
static inline void set_bit_atn(uint64_t* phy_idx,  uint8_t n){
    *phy_idx = *phy_idx | (1ull << n);
}
static inline uint8_t get_bit_atn(uint64_t* phy_idx,  uint8_t n){
    return (*phy_idx >> n) & 1; 
}


static inline void clear_bit_at(phy2_t* p2, phy_idx* idx){
    p2->entry[idx->idx] =  p2->entry[idx->idx] & ~(1ull << idx->bit);
}

static inline uint8_t count_set_bits(uint64_t n) {
    uint8_t count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}