#pragma once
#include "../stdlib.h"

/*
Let's Makeup a Filesystem

aw man turns out we wanna be somewhat POSIX adjacent
*/

#define VFS_NAME_LEN 128
#define VFS_DIR_MAX_ENTRIES 128

enum VFS_TYPES
{
    VFS_FILE = 0u,
    VFS_DIR,
    VFS_ROOT,
    VFS_SYMLINK, //yeah im only implementingup to here for now
    VFS_PIPE,
    VFS_CHARSTREAM, //stdout
    VFS_DEV,
    VFS_DEVIDK,
    VFS_DEVWTF,
};

typedef struct // POSIX.
{
  char name[VFS_NAME_LEN]; // Filename.
  uint32_t inode;     // Inode 
} dir_entry;

typedef struct {
    uint8_t D : 1; //is directory
    uint8_t R : 1; //read
    uint8_t W : 1; //write
    uint8_t X : 1; //execute
    uint8_t lvl : 2; //0-3 for access level eql or lower 
    uint8_t _zero : 2; //save for later 
}__attribute__((packed)) vfs_perms;
static_assert(sizeof(vfs_perms) == 1, "vfs_perms size not what you want!");

struct vfs_node;
                                //node      file/buf offset   size         buffer to read to write from
typedef uint32_t (*vfs_read_fn)(struct vfs_node*, uint64_t , size_t, uint8_t*);
typedef uint32_t (*vfs_write_fn)(struct vfs_node*, uint64_t , size_t, uint8_t*);
typedef void (*vfs_open_fn)(struct vfs_node*);
typedef void (*vfs_close_fn)(struct vfs_node*);
typedef dir_entry* (*vfs_readdir_fn)(struct vfs_node*, uint32_t);
typedef struct vfs_node* (*vfs_finddir_fn)(struct vfs_node*, const char*);

typedef struct {
    union {
        vfs_read_fn read;
        vfs_readdir_fn readdir;
    };
    union {
        vfs_write_fn write;
        vfs_finddir_fn finddir;
    };
    vfs_close_fn close;
    vfs_open_fn open;
} vfs_fns;

typedef struct {
    char name[VFS_NAME_LEN];
    uint8_t type;
    vfs_perms perm;
    vfs_fns fn; 

    uint32_t inode;
    size_t length;
    uint8_t impl;

    void* p; //sym link or head node or dirinfo idfk
} __attribute__((packed)) vfs_node;


typedef struct {
    uint32_t dir_inode; //idk if ill use this but i suspect life will be easier if I do
    uint8_t size;
    uint32_t entries[VFS_DIR_MAX_ENTRIES];

    vfs_node* parent; //owner 
} __attribute__((packed)) vfs_dir_info;


extern vfs_node* vfs_root;

vfs_node* init_vfs(uintptr_t ptr, size_t size);
