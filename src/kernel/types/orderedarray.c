#include <kernel/types/orderedarray.h>

#include <kernel/printk.h>

void oarray_print_info(oarray_t* map){
    debugf("oarray [ %lx ] size=%u \n",(uintptr_t)(map), map->idx);
    debugf("max size = %u max_size_bytes = %lx \n",map->prop.max_elements, map->prop.max_size_bytes);
}

void oarray_print_contents(oarray_t* map, oarray_print_element_fn print_element){
    debugf("oarray [ %lx ] dumping %u elements \n",(uintptr_t)(map), map->idx);
    for(int i = 0; i < map->idx; ++i){
        debugf("\n [%i] -> ");
        print_element(map->items[i]);

    }

    debugf("\n ==oarray dump END== \n");
}