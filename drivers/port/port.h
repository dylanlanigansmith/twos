#pragma once
#include "../../kernel/types/stdint.h"

uint8_t port_byte_in(uint16_t port);

void port_byte_out(uint16_t port, uint8_t data);

uint16_t port_word_in(uint16_t port);

void port_word_out(uint16_t port, uint16_t data);


static inline void io_wait(){
    //use unused port to kill time, linux kernel also uses 0x80
    port_byte_out(0x80, 0);
}