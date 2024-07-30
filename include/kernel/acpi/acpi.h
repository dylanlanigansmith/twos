#pragma once
#include <kstdlib.h>

#include "rsdp.h"


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
