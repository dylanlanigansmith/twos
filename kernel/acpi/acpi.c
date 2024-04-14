#include "acpi.h"
#include "../sys/sysinfo.h"


    //rsdt ptr is virt addr, map base is page aligned virt addr, rsdt phys is from rsdp 
int ACPI_discover_SDTs(void* rsdt_ptr, uintptr_t map_base, uintptr_t rsdt_phys){

    //this bad boy adds em to sysinfo too 
    RSDT* rsdt = (RSDT*) rsdt_ptr;

    int table_entries = (rsdt->header.Length - sizeof(ACPISDTHeader)) / sizeof(uint32_t);
    int crc = validate_acpi(rsdt, sizeof(ACPISDTHeader) + sizeof(uint32_t) * table_entries); //dunno

    if(!crc){
         debugf("RSDT checksum failed @%lx, claimed %i entries", rsdt_ptr, table_entries); return 0;
    }
  
    char hsig[5] = {0,0,0,0,0}; //for null term
    memcpy(hsig, rsdt->header.Signature, 4);
    debugf("RSDT %s claims %i entries \n",hsig, table_entries);

    for(int i = 0; i <  table_entries; ++i){
         
        uint32_t header_addr32 =  rsdt->SDTs[i];

        uintptr_t header_phys = header_addr32;
        debugf("[%i / %i]  = %lx\n",i, table_entries, header_phys);

        uint64_t offset = (rsdt_phys > header_phys) ? (rsdt_phys - header_phys) : (header_phys - rsdt_phys);
        //lets just not worry about header being > rsdt rn

        uintptr_t header_virt = ( uintptr_t) rsdt_ptr - offset; 
        debugf("[%i ] virtual is %lx, offset was - %lx \n", i, header_virt, offset);
        ASSERT(header_virt > map_base); 
        ACPISDTHeader* header = (ACPISDTHeader*)(header_virt);

        if(header){
            //check crc again

             
             int hcrc = validate_acpi(header, header->Length);
            char sig[5] = {0,0,0,0,0}; //for null term
            memcpy(sig, header->Signature, 4);
            debugf("found table entry [%i] = '%s' crc = %s \n",i, sig, (hcrc) ? "OK" : "FAIL");

            if( i < ACPI_ENTRIES_MAX && crc) {


                sysinfo.rsdt_entries[i] =  header_virt;
                if(!strncmp(sig, "FACP", 4)){
                    sysinfo.fadt_idx = i;
                    debugf("added %s to sysinfo[%i], marked FACP idx %i\n", sig, i, sysinfo.fadt_idx);
                }
                else debugf("added %s to sysinfo[%i]\n", sig, i);
                
            }
            
          //  if(idx >= ACPI_ENTRIES_MAX) panic("more ACPI than we bargined for!");
            
            
            
        }
    }


    return table_entries;

}