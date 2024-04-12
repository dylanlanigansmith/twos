#pragma once
#include "../stdlib/stdint.h"


//rsdp_t->Revision
#define ACPI_V1 0
#define ACPI_V2 2

//from https://wiki.osdev.org/RSDP
typedef struct  {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;
} __attribute__ ((packed)) RSDP_t;


//v2 for multiboot_tag_acpi_new
typedef struct  {
 char Signature[8];
 uint8_t Checksum;
 char OEMID[6];
 uint8_t Revision;
 uint32_t RsdtAddress;      // deprecated since version 2.0
 
 uint32_t Length;
 uint64_t XsdtAddress;
 uint8_t ExtendedChecksum;
 uint8_t reserved[3];
} __attribute__ ((packed)) XSDP_t; //currently unsupported