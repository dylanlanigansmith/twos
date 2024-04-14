#pragma once
#include "../stdlib.h"


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


static inline int validate_rsdp(RSDP_t* rsdp){
    if(!rsdp) return 0;

    uint8_t sum = 0;
    for(uint32_t i = 0; i < sizeof(RSDP_t); ++i){
        sum += ( (uint8_t*) rsdp)[i];
    }

    return sum == 0;
}

static inline int validate_acpi(void* sdt, size_t size){
    if(!sdt) return 0;

    uint8_t sum = 0;
    for(uint32_t i = 0; i < size; ++i){
        sum += ( (uint8_t*) sdt)[i];
    }

    return sum == 0;
}
/*

//osdev wiki is just straight wrong about this above impl is fine
static inline int validate_rsdp(RSDP_t* rsdp){
    if(!rsdp) return 0;

   
    uint8_t chk = rsdp->Checksum;
    uint32_t sum = 0;
    for(uint32_t i = 0; i < sizeof(RSDP_t); ++i){
        sum += ( (uint8_t*) rsdp)[i];
    }
     debugf("chk = %i sum = %i cast = %i \n", chk, sum, (uint8_t)(sum));
    return (uint8_t)(sum) == 0;
}*/