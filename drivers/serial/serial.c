#include "serial.h"
#include "../port/port.h"


static bool serial__has_init = False;

#define CHECK_SERIAL_INIT() if(!serial__has_init) return;

int serial_init()
{
    serial__has_init = False;
   port_byte_out(SERIAL_PORT_COM1 + 1, 0x00);    // Disable all interrupts
   port_byte_out(SERIAL_PORT_COM1 + 3, 0x80);    // Enable DLAB (set baud rate divisor)
   port_byte_out(SERIAL_PORT_COM1 + 0, 0x01);    // Set divisor to 3 (lo byte) 38400 baud
   port_byte_out(SERIAL_PORT_COM1 + 1, 0x00);    //                  (hi byte)
   port_byte_out(SERIAL_PORT_COM1 + 3, 0x02);    // 8 bits, no parity, one stop bit
   port_byte_out(SERIAL_PORT_COM1 + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
   port_byte_out(SERIAL_PORT_COM1 + 4, 0x0B);    // IRQs enabled, RTS/DSR set
   port_byte_out(SERIAL_PORT_COM1 + 4, 0x1E);    // Set in loopback mode, test the serial chip
   port_byte_out(SERIAL_PORT_COM1 + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)
 
   // Check if serial is faulty (i.e: not same byte as sent)
   if(port_byte_in(SERIAL_PORT_COM1 + 0) != 0xAE) {
      return 1;
   }
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   port_byte_out(SERIAL_PORT_COM1 + 4, 0x0F);
   serial__has_init = True;
   return 0;
}


int is_transmit_empty(){
    return port_byte_in(SERIAL_PORT_COM1 + 5) & 0x20;
}

void serial_write(char a)
{
    while(is_transmit_empty() == 0);
        port_byte_out(SERIAL_PORT_COM1, a);
   
    
}
 #define BOCHS
void serial_print(const char *str)
{
     #ifdef BOCHS
    for(int c = 0; str[c] != 0; ++c)
        port_e9_hack_out(str[c]);
    
    #else
    CHECK_SERIAL_INIT()
    for(int c = 0; str[c] != 0; ++c)
        serial_write(str[c]);


   #endif
}

void serial_println(const char *str)
{
    serial_print(str);
    serial_write('\n');
}


void serial_printi(const char *str, int64_t i)
{
    serial_print(str);
    serial_print(": ");
    serial_print(lltoa(i, 10));
    serial_print(" \n");
}

void serial_printh(const char *str, int64_t i)
{
    serial_print(str);
    serial_print(": 0x");
    serial_print(lltoa(i, 16));
    serial_print(" \n");
}
