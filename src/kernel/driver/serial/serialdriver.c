#include <kernel/driver/serial/serialdriver.h>

#include <kernel/port/port.h>

static uint8_t serial__has_init = 0;

#define CHECK_SERIAL_INIT() if(unlikely(!serial__has_init)) return;


static long use_e9 = 0lu;

int serial_init()
{
    if(serial__has_init) return 0;

    serial__has_init = 0;
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

    //set up serial anyways with a prayer
        serial__has_init = 1;
      return 1;
   }
 
   // If serial is not faulty set it in normal operation mode
   // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
   port_byte_out(SERIAL_PORT_COM1 + 4, 0x0F);
   serial__has_init = 1;
   use_e9 = 0lu; //should be thru sysinfo but we check that really late argh catch 22
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

void serial_print(const char *str)
{
    CHECK_SERIAL_INIT();

    if(likely(use_e9 == 0)){ 
        
        for(int c = 0; str[c] != 0; ++c)
            serial_write(str[c]);

        return;
    }


     //apparently we can read 0xe9 and check for bochs
    for(int c = 0; str[c] != 0; ++c)
        _port_e9_hack_out(str[c]);

    
    //port_e9_hack_out(0);

}

void serial_println(const char *str)
{
    serial_print(str);
    serial_print("\n");
}



void serial_set_e9()
{
    use_e9 = 1;
}