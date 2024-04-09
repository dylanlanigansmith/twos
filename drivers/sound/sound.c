#include "sound.h"

#include "../port/port.h"
void play_sound(uint32_t nFrequence) //lifted straight from wiki for debugging
{
    uint32_t Div;
    uint8_t tmp;

    // Set the PIT to the desired frequency
    Div = 1193180 / nFrequence;
    port_byte_out(0x43, 0xb6);
    port_byte_out(0x42, (uint8_t)(Div));
    port_byte_out(0x42, (uint8_t)(Div >> 8));

    // And play the sound using the PC speaker
    tmp = port_byte_in(0x61);
    if (tmp != (tmp | 3))
    {
        port_byte_out(0x61, tmp | 3);
    }
}
void nosound()
{
    uint8_t tmp = port_byte_in(0x61) & 0xFC;

    port_byte_out(0x61, tmp);
}