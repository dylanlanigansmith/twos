#pragma once
#include <kstdlib.h>

typedef uintptr_t oarray_element_t; 

typedef void* (*oarray_malloc_fn)(size_t );
typedef size_t (*oarray_mfree_fn)(void* );

typedef void (*oarray_print_element_fn)(oarray_element_t* );


typedef uint8_t* (*oarray_greater_fn)(oarray_element_t*, oarray_element_t*);


//wish i picked elements or items and just stuck to it
typedef struct {
    

    oarray_malloc_fn bmalloc;
    oarray_mfree_fn bfree;
    oarray_greater_fn bgreater;
    struct{
        uint64_t max_size_bytes;
        uint32_t  max_elements;
    }prop;
    
    uint32_t idx; //aka num elements
    oarray_element_t* items[0];

} oarray_t;




static inline oarray_t* oarray_init(uintptr_t size, oarray_malloc_fn our_malloc, oarray_mfree_fn our_free, oarray_greater_fn our_greater){
    oarray_t* map = our_malloc(size);
    map->prop.max_size_bytes = size;
    map->bgreater = our_greater;
    map->bfree = our_free;
    map->bmalloc = our_malloc;
    map->prop.max_elements = (size - sizeof(oarray_t)) / sizeof(oarray_element_t* ) ;  //(size - (2 * (sizeof(oarray_t) ) ) ) / sizeof(oarray_element_t*); //this is broken
    map->idx = 0;
    return map;
}

static inline void oarray_destroy(oarray_t* map){
    map->bfree(map);
}

static inline oarray_element_t* oarray_insert(oarray_t* map, oarray_element_t* element) {
    if(map->idx >= map->prop.max_elements)
        return 0;
    uint32_t itr = 0;
    while(itr < map->idx && !map->bgreater(map->items[itr], element))
        itr++;

   
    if(itr == map->idx) {// we hit end, its the largest!
        map->items[map->idx++] = element;
    }
    else{
         
        //we gotta insert in the middle
        oarray_element_t* displaced = map->items[itr];
        map->items[itr] = element;

        while(itr < map->idx){
            itr++;
            oarray_element_t* tmp = map->items[itr];
            map->items[itr] = displaced;
            displaced = tmp;
        }
        map->idx++;
    }

    return element;
}

static inline uint32_t oarray_remove(oarray_t* map, uint32_t idx){
    while(idx < map->idx){
        map->items[idx] = map->items[idx+1];
        idx++;
    }
    map->idx--;
}

static inline oarray_element_t* oarray_get(oarray_t* map, uint32_t idx){
    if(idx > map->idx) return 0;

    return map->items[idx];
}



void oarray_print_info(oarray_t* map);

void oarray_print_contents(oarray_t* map, oarray_print_element_fn print_element);