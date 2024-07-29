#include <kernel/printk.h>
#include <kernel/port/port.h>
#include <kstdlib/print.h>
#include <kernel/driver/drivers.h>
void _printk_vm(char *str)
{
    while(*str != '\0') _port_e9_hack_out(*(str++));
}

void _printk_findoutputs(const char* str){
    serial_println(str);
    //if() _printk_vm(str);
}


uint8_t printk(uint8_t level, const char *fmt, ...)
{
    //router to different print functions basically
    //for now we can just assume 

    #define PRINTK_BUF_SIZE 2048
    char buf[2048];
    
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf,  PRINTK_BUF_SIZE , fmt, args);
    va_end(args);

    _printk_findoutputs(buf);
    return 0;
}


