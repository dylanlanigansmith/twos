#pragma once
#include "stdint.h"

#include "../common/video.h"


void* set_gfx_mode(); 

void* gfx_clear(uint32_t argb); 
void* gfx_load_info(gfx_info_t* gi);
extern gfx_info_t* gfxinfo;

gfx_info_t* gfx_get_info();