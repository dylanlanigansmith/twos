#pragma once
#include "../stdlib.h"
#include "multiboot2.h"
#define CMDLINE_MAX_LEN 64

#define CMDLINE_MAX_ARG 16
#define CMDLINE_MAX_ARG_LEN 16

#define CMDLINE_NO_ARG UINT32_MAX

size_t cmdline_numargs();

int cmdline_parse(struct multiboot_tag_string* stag);

bool cmdline_hasarg(const char* arg);

uint32_t cmdline_getarg(const char* arg);

bool cmdline_is_true(const char* arg); //we call this knowing arg is 0/1 so we dont care if it doesnt exist 