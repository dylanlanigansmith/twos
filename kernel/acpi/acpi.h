#pragma once
#include "rdsp.h"
#include "../kernel.h"
//from https://wiki.osdev.org/RSDT so probs wrong
#define ACPI_ADDR 0xffff800000000000llu

typedef struct  {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
}  __attribute__ ((packed)) ACPISDTHeader;


typedef struct {
    ACPISDTHeader header;
    uint32_t SDTs[0]; // sdt[ (header.length - sizeof(ACPISDTHeader) / 8 ] 
} RSDT;


                        //rsdt ptr is virt addr, map base is page aligned virt addr, rsdt phys is from rsdp 
int ACPI_discover_SDTs(void* rsdt_ptr, uintptr_t map_base, uintptr_t rsdt_phys);