#pragma once
#include "../../kernel/stdlib/stdint.h"
//https://wiki.osdev.org/VESA_Video_Modes
typedef struct  {
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
} __attribute__ ((packed)) vbe_mode_info_t;

/*
 if(tagvbe && 0){
        vbe_mode_info_t* vbe = (vbe_mode_info_t*)tagvbe->vbe_mode_info.external_specification;
        serial_printi("vbe->attributes", vbe->attributes);
        serial_printi("vbe->window_a", vbe->window_a);
        serial_printi("vbe->window_b", vbe->window_b);
        serial_printi("vbe->granularity", vbe->granularity);
        serial_printi("vbe->window_size", vbe->window_size);
        serial_printi("vbe->segment_a", vbe->segment_a);
        serial_printi("vbe->segment_b", vbe->segment_b);
        serial_printi("vbe->win_func_ptr", vbe->win_func_ptr);
        serial_printi("vbe->pitch", vbe->pitch);
        serial_printi("vbe->width", vbe->width);
        serial_printi("vbe->height", vbe->height);
        serial_printi("vbe->w_char", vbe->w_char);
        serial_printi("vbe->y_char", vbe->y_char);
        serial_printi("vbe->planes", vbe->planes);
        serial_printi("vbe->bpp", vbe->bpp);
        serial_printi("vbe->banks", vbe->banks);
        serial_printi("vbe->memory_model", vbe->memory_model);
        serial_printi("vbe->bank_size", vbe->bank_size);
        serial_printi("vbe->image_pages", vbe->image_pages);
        serial_printi("vbe->reserved0", vbe->reserved0);
        serial_printi("vbe->red_mask", vbe->red_mask);
        serial_printi("vbe->red_position", vbe->red_position);
        serial_printi("vbe->green_mask", vbe->green_mask);
        serial_printi("vbe->green_position", vbe->green_position);
        serial_printi("vbe->blue_mask", vbe->blue_mask);
        serial_printi("vbe->blue_position", vbe->blue_position);
        serial_printi("vbe->reserved_mask", vbe->reserved_mask);
        serial_printi("vbe->reserved_position", vbe->reserved_position);
        serial_printi("vbe->direct_color_attributes", vbe->direct_color_attributes);
        serial_printh("vbe->framebuffer", vbe->framebuffer);
        serial_printi("vbe->off_screen_mem_off", vbe->off_screen_mem_off);
        serial_printi("vbe->off_screen_mem_size", vbe->off_screen_mem_size);
// Note: Printing reserved1, which is an array, might not provide useful information in debug prints

    }


*/