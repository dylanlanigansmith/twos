#include "../kernel.h"

#include "../boot/multiboot2.h"

#define MB_HEADER_PARSE_ERROR 1 
#define MB_HEADER_PARSE_SUCCESS 0

int get_multiboot_initial_info(void* header, uint64_t magic);

int parse_multiboot_header(void* header, uint64_t magic);

#define DUMP_VBE_TAG 0
#define DUMP_FB_TAG 0
#define DUMP_ACPI_TAG 0

#define DUMP_BASIC_MEM_TAG 0

#define DUMP_MMAP_TAG 0

#define DUMP_MODULE_TAG 0

#define DUMP_UNKNOWN_TAGS 1