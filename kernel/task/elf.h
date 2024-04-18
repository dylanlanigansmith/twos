#include "../stdlib.h"

#include "../fs/vfs.h"

typedef uintptr_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;

typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;

typedef uint64_t Elf64_Xword;
typedef uint64_t Elf64_Sxword;

typedef struct
{
    unsigned char e_ident[16]; /* ELF identification */
    Elf64_Half e_type;         /* Object file type */
    Elf64_Half e_machine;      /* Machine type */
    Elf64_Word e_version;      /* Object file version */
    Elf64_Addr e_entry;        /* Entry point address */
    Elf64_Off e_phoff;         /* Program header offset */
    Elf64_Off e_shoff;         /* Section header offset */
    Elf64_Word e_flags;        /* Processor-specific flags */
    Elf64_Half e_ehsize;       /* ELF header size */
    Elf64_Half e_phentsize;    /* Size of program header entry */
    Elf64_Half e_phnum;        /* Number of program header entries */
    Elf64_Half e_shentsize;    /* Size of section header entry */
    Elf64_Half e_shnum;        /* Number of section header entries */
    Elf64_Half e_shstrndx;     /* Section name string table index */
} Elf64_Ehdr;





typedef struct
{
    Elf64_Word sh_name;       /* Section name */
    Elf64_Word sh_type;       /* Section type */
    Elf64_Xword sh_flags;     /* Section attributes */
    Elf64_Addr sh_addr;       /* Virtual address in memory */
    Elf64_Off sh_offset;      /* Offset in file */
    Elf64_Xword sh_size;      /* Size of section */
    Elf64_Word sh_link;       /* Link to other section */
    Elf64_Word sh_info;       /* Miscellaneous information */
    Elf64_Xword sh_addralign; /* Address alignment boundary */
    Elf64_Xword sh_entsize;   /* Size of entries, if section has table */
} Elf64_Shdr;

typedef struct
{
    Elf64_Word st_name;     /* Symbol name */
    unsigned char st_info;  /* Type and Binding attributes */
    unsigned char st_other; /* Reserved */
    Elf64_Half st_shndx;    /* Section table index */
    Elf64_Addr st_value;    /* Symbol value */
    Elf64_Xword st_size;    /* Size of object (e.g., common) */
} Elf64_Sym;



typedef struct
{
    Elf64_Addr r_offset; /* Address of reference */
    Elf64_Xword r_info;  /* Symbol index and type of relocation */
} Elf64_Rel;



typedef struct
{
    Elf64_Addr r_offset;   /* Address of reference */
    Elf64_Xword r_info;    /* Symbol index and type of relocation */
    Elf64_Sxword r_addend; /* Constant part of expression */
} Elf64_Rela;


typedef struct
{
    Elf64_Word p_type;    /* Type of segment */
    Elf64_Word p_flags;   /* Segment attributes */
    Elf64_Off p_offset;   /* Offset in file */
    Elf64_Addr p_vaddr;   /* Virtual address in memory */
    Elf64_Addr p_paddr;   /* Reserved */
    Elf64_Xword p_filesz; /* Size of segment in file */
    Elf64_Xword p_memsz;  /* Size of segment in memory */
    Elf64_Xword p_align;  /* Alignment of segment */
} Elf64_Phdr;
#define PT_NULL 0 ///Unused entry
#define PT_LOAD 1 //Loadable segment
#define PT_DYNAMIC 2 //Dynamic linking tables
#define PT_INTERP 3 //Program interpreter path name
#define PT_NOTE 4 //Note sections
#define PT_SHLIB 5// Reserved
#define PT_PHDR 6 //Program header table
#define PT_LOOS 0x60000000 //Environment-specific use
#define PT_HIOS 0x6FFFFFFF
#define PT_LOPROC 0x70000000 //Processor-specific use
#define PT_HIPROC 0x7FFFFFFF













//Elf header defs
enum ELF_Ident
{
    EI_MAG0 = 0,   // File identification
    EI_MAG1,       // 1
    EI_MAG2,       // 2
    EI_MAG3,       // 3
    EI_CLASS,      // 4 //File class
    EI_DATA,       // 5 Data encoding
    EI_VERSION,    // 6 File version
    EI_OSABI,      // 7 OS/ABI identification
    EI_ABIVERSION, // 8 ABI version
    EI_PAD,        // 9 Start of padding bytes
    EI_NIDENT = 16 // 16 Size of e_ident[]
};


#define ELFDATA2LSB 1 //little endian
#define ELFDATA2MSB 2

#define ELFOSABI_SYSTEMV 0
#define ELFOSABI_STANDALONE 255

#define ET_NONE 0 //No file type
#define ET_REL 1 //Relocatable object file
#define ET_EXEC 2 //Executable file
#define ET_DYN 3 //Shared object file
#define ET_CORE 4 //Core file
#define ET_LOOS 0xFE00 //Environment-specific use
#define ET_HIOS 0xFEFF
#define ET_LOPROC 0xFF00 //Processor-specific use
#define ET_HIPROC 0xFFFF


#define ELFCLASS32 1 //32-bit objects
#define ELFCLASS64 2 //64-bit objects

#define ET_NONE 0 //No file type
#define ET_REL 1 //Relocatable object file
#define ET_EXEC 2 //Executable file
#define ET_DYN 3 //Shared object file
#define ET_CORE 4 //Core file
#define ET_LOOS 0xFE00 //Environment-specific use
#define ET_HIOS 0xFEFF
#define ET_LOPROC 0xFF00 //Processor-specific use
#define ET_HIPROC 0xFFFF





static inline unsigned long elf64_hash(const unsigned char *name)          
{
    unsigned long h = 0, g;
    while (*name)
    {
        h = (h << 4) + *name++;
        if (g = h & 0xf0000000)
            h ^= g >> 24;
        h &= 0x0fffffff;
    }
    return h;
}

uint64_t load_elf(vfs_node* file);