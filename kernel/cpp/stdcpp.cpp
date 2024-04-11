#include "stdcpp.hpp"

void* operator new(size_t size) {
    return kmalloc(size);
}

// Override the global delete operator
void operator delete(void* ptr) noexcept {
    kfree(ptr);
}

// Override the array new operator
void* operator new[](size_t size) {
    return kmalloc(size);
}

// Override the array delete operator
void operator delete[](void* ptr) noexcept {
    kfree(ptr);
}

