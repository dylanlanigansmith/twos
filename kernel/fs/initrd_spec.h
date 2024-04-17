#pragma once


#ifndef __cplusplus
#include "../stdlib.h"
#else
//this is not shared with native OS cpp code, but the initrd utils linux program
#include <cstdint>
extern "C" {

#endif
/*
rd file

[0x00 ] header
-dir info
-dir info
-dir info
..etc..
[sizeof(header) + sizeof(dirinfo) * numdirs] file_header 1
-file header
-file header
-file header

footer just for personal reasons
*/

#define INITRD_CRC 0xCAFEBABE
#define INITRD_ROOT 0
typedef struct {
    uint32_t num_files;
    uint32_t num_dirs;
    uint32_t root_inode;
    uint32_t crc;
}__attribute__((packed)) initrd_header;

typedef struct {
    size_t size; // not including footer
    uint32_t crc;
}__attribute__((packed)) initrd_footer;
/*
every file has an index for what dir it belongs in
we call em inodes but will have to change to vfs stuff later

*/

typedef struct {
    uint32_t parent_inode;
    uint32_t dir_inode;
    uint8_t name[128];
    uint32_t crc;
}__attribute__((packed)) initrd_dir_info;

typedef struct {
    uint8_t name[128];
    uint64_t offset; //from 0x0 of initrd file
    size_t size;
    
    uint32_t parent_dir;
    uint8_t perms;
    uint32_t _zero; //reserved
    uint32_t crc;
}__attribute__((packed)) initrd_file_header;

#ifdef __cplusplus
}
#endif