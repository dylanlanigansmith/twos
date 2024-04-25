#pragma once
#include "stdint.h"
#include "print.h"

//god i wish this was c++
//this isnt a map its an ordered array

typedef void bmap_element_t; //i like seeing reminder of pointers so didnt do void* 

typedef void* (*bmap_malloc_fn)(void* );
typedef size_t (*bmap_mfree_fn)(void* );

typedef void (*bmap_print_element_fn)(bmap_element_t* );


typedef uint8_t* (*bmap_greater_fn)(bmap_element_t*, bmap_element_t*);


//wish i picked elements or items and just stuck to it
typedef struct {
    

    bmap_malloc_fn bmalloc;
    bmap_mfree_fn bfree;
    bmap_greater_fn bgreater;
    struct{
        uint64_t max_size_bytes;
        uint32_t  max_size;
    }prop;
    
    uint32_t size; //aka num elements
    bmap_element_t* items[1]; //this is broken

} bmap_t; //bmap = bad map

//int i = sizeof(bmap_t);


static inline bmap_t* bmap_init(uintptr_t size, bmap_malloc_fn our_malloc, bmap_mfree_fn our_free, bmap_greater_fn our_greater){
    bmap_t* map = our_malloc(size);
    map->prop.max_size_bytes = size;
    map->bgreater = our_greater;
    map->bfree = our_free;
    map->bmalloc = our_malloc;
    map->prop.max_size = (size - (2 * (sizeof(bmap_t) ) ) ) / sizeof(bmap_element_t*); //this is broken
    map->size = 0;
    return map;
}

static inline void bmap_destroy(bmap_t* map){
    //nahhhh
    map->bfree(map);
}

static inline bmap_element_t* bmap_insert(bmap_t* map, bmap_element_t* element) {
    if(map->size >= map->prop.max_size)
        return 0;
    uint32_t itr = 0;
    while(itr < map->size && !map->bgreater(map->items[itr], element))
        itr++;

   
    if(itr == map->size) {// we hit end, its the largest!
        map->items[map->size++] = element;
    }
    else{
         
        //we gotta insert in the middle
        bmap_element_t* displaced = map->items[itr];
        map->items[itr] = element;

        while(itr < map->size){
            itr++;
            bmap_element_t* tmp = map->items[itr];
            map->items[itr] = displaced;
            displaced = tmp;
        }
        map->size++;
    }

    return element;
}

static inline uint32_t bmap_remove(bmap_t* map, uint32_t idx){
    while(idx < map->size){
        map->items[idx] = map->items[idx+1];
        idx++;
    }
    map->size--;
}

static inline bmap_element_t* bmap_get(bmap_t* map, uint32_t idx){
    if(idx > map->size) return nullptr;

    return map->items[idx];
}

static inline void bmap_print_info(bmap_t* map){
    printf("bmap [ %lx ] size=%u \n",(uintptr_t)(map), map->size);
    printf("max size = %u max_size_bytes = %lx \n",map->prop.max_size, map->prop.max_size_bytes);
}

static inline void bmap_print_contents(bmap_t* map, bmap_print_element_fn print_element){
    printf("bmap [ %lx ] dumping %u elements \n",(uintptr_t)(map), map->size);
    for(int i = 0; i < map->size; ++i){
        printf("\n [%i] -> ");
        print_element(map->items[i]);

    }

    print("\n ==bmap dump END== \n");
}