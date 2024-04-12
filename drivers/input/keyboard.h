#pragma once
#include "../../kernel/stdlib.h"

extern uint8_t key_queue[64];

void keyboard_init();


uint8_t* get_keys(uint8_t* amt);

bool keys_available();

void keys_lock_queue();

void keys_reset_queue();