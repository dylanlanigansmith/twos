#include <kstdlib/kassert.h>

#include <kernel/printk.h>
void _assert_try_print(const char* str, const char* file, int line){
    debugf("assertion failed: '%s' !!! \n-->%s (%d)", str, file, line);
}