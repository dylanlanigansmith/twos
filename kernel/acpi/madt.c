#include "madt.h"

#include "../sys/sysinfo.h"
/*
god i hate ACPI already

also who went with acpi and APIC 
shit is confusing for my dyslexic ass 

*/

struct {
    madt_proc_local_apic* proc_local_apic;  //0
    madt_io_apic* io_apic;                  //1
    madt_io_apic_src_override* io_apic_iso; //2
    madt_apic_x64_override* apic_addr64; 

    //5
} madt_entries;

void madt_discover()
{

    debug("\n===MADT===  \n");
    memset(&madt_entries, 0, sizeof(madt_entries));

    MADT_t* madt = (MADT_t*)sysinfo.rsdt_entries[sysinfo.madt_idx];
    debugf("madt discover: apic = %i ", madt->apic_address);

    if(madt->pic_flags & (0b001)){
        debug("Legacy PICs available and must be disabled to use APIC");
    }
    
    if(!madt->entry.entry_length){
        debug("madt no first entry!?");
        return;
    }
    //we want a type 5 definitely, a 2, 

    MADT_entry_descriptor* entry = &madt->entry;
    int num_entries = 0;
    while(entry){
        num_entries++;
        debugf("madt entry [%i] size %i #%i ", entry->entry_type, entry->entry_length, num_entries);
        
        switch(entry->entry_type){
            case MADT_PROC_LOCAL_APIC:
                madt_entries.proc_local_apic = ( madt_proc_local_apic* )( (uintptr_t)entry ); break;
            case MADT_IO_APIC:
                madt_entries.io_apic = (madt_io_apic*)( (uintptr_t) entry ) ;
                //can be more than one of these
                
                 debugf("madt io_apic id = %i addr =%x gsi = %lx", madt_entries.io_apic->ioapic_id, madt_entries.io_apic->addr, madt_entries.io_apic->gsi_base);
                 break ; 
            case MADT_IO_APIC_ISO:
            //so we actually wanna register all these and not just store one 
                madt_entries.io_apic_iso = (madt_io_apic_src_override* )( (uintptr_t) entry ) ;
                debugf("madt io_apic_override: bussrc = %i irqsrc =%i gsi = %i, flags: %b", madt_entries.io_apic_iso->bus_src, madt_entries.io_apic_iso->irq_src, madt_entries.io_apic_iso->gsi, madt_entries.io_apic_iso->flags);
                break ;
            case  MADT_LOCAL_APIC_ADDR_OVERRIDE:
                madt_entries.apic_addr64 = (madt_apic_x64_override*)( (uintptr_t) entry ) ; break ;
            
            default:
                debugf("madt entry type [%i] len %i unknown ", entry->entry_type, entry->entry_length); break;
        }

                                        //we dont need the plus one this works fine, but lets keep it for sake of hope 
        if((uintptr_t)entry + entry->entry_length + 1 >= (uintptr_t)madt + madt->header.Length ){
            debugf("last madt at %lx size %i. madt at %lx, size %i \n", (uintptr_t)entry, entry->entry_length, (uintptr_t)madt, madt->header.Length  );
            debugf("entry end at %lx  madt end at %lx\n", (uintptr_t)entry + entry->entry_length, (uintptr_t)madt + madt->header.Length  );
            debugf("MADT: ending on madt entry %i, total %i", entry->entry_type, num_entries); entry = 0; break;
        } //call it a day

        entry = (MADT_entry_descriptor*) ((uintptr_t)entry + (uintptr_t)entry->entry_length);
    }
    if(madt_entries.apic_addr64){
        debugf("madt apic addr64 = %lx", madt_entries.apic_addr64->phys_addr);
    }
    if(madt_entries.io_apic){
        debugf("madt io_apic id = %i addr =%x gsi = %lx", madt_entries.io_apic->ioapic_id, madt_entries.io_apic->addr, madt_entries.io_apic->gsi_base);
    }
    
    debug("===end madt=== \n \n");
}
