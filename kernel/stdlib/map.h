#pragma once
#include "stdint.h"

//god i wish this was c++

typedef void* (*bmap_malloc_fn)(uintptr_t );
typedef uint64_t (*bmap_mfree_fn)(uintptr_t );

typedef struct {
    struct bmap_element{
        uintptr_t node;
    }; 

    bmap_malloc_fn bmalloc;
    bmap_mfree_fn bfree;

    struct{
        uint64_t max_size;
    }mem;
} bmap_t; //bmap = bad map


bmap_t* bmap_init(uintptr_t size, bmap_malloc_fn ourmalloc, bmap_mfree_fn our_free){
    bmap_t* map = ourmalloc(size);

    return map;
}