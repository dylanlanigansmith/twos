#pragma once
#include "rdsp.h"
#include "../kernel.h"
//from https://wiki.osdev.org/RSDT so probs wrong


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
    uintptr_t* SDTs; // sdt[ (header.length - sizeof(ACPISDTHeader) / 8 ] 
} RSDT;


static inline void* ACPI_findSDT(void* rsdt_ptr, const char* sig){
    RSDT* rsdt = (RSDT*) rsdt_ptr;

    int table_entries = (rsdt->header.Length - sizeof(ACPISDTHeader)) / sizeof(uintptr_t);

    for(int i = 0; i <  table_entries; ++i){
        ACPISDTHeader* header =  (ACPISDTHeader*)rsdt->SDTs[i];
        if(!header) continue;
        if(!strncmp(header->Signature, sig, 4))
            return (void*)header;

    }
    return nullptr;
}

static inline void ACPI_discover_SDTs(void* rsdt_ptr){
    RSDT* rsdt = (RSDT*) rsdt_ptr;

    int table_entries = (rsdt->header.Length - sizeof(ACPISDTHeader)) / sizeof(uintptr_t);
     debugf("RSDT claims %i entries \n", table_entries);
    for(int i = 0; i <  table_entries; ++i){
        ACPISDTHeader* header =  (ACPISDTHeader*)rsdt->SDTs[i];
        if(!header) continue;
        char sig[5] = {0,0,0,0,0}; //for null term
        memcpy(sig, header->Signature, 4);
        debugf("table entry [%i] = '%s' \n", sig);

    }

}