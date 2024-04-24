#pragma once
#include "../../kernel/stdlib.h"

extern uint8_t key_queue[64];

extern uint64_t last_key_event;

void keyboard_init();


uint8_t* get_keys(uint8_t* amt);

bool keys_available();

void keys_lock_queue();

void keys_reset_queue();


uint8_t is_key_down_ascii(char c);
uint8_t is_key_down(uint8_t sc);


uint8_t  keys_last_event();

