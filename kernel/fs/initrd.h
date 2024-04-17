#pragma once
#include "initrd_spec.h"
#include "vfs.h"


int initrd(uintptr_t addr, size_t size);


void initrd_demo();

vfs_node* initrd_find_root(uint32_t inode);

extern vfs_node* initrd_root;

vfs_node * initrd_findfile(vfs_node* root, const char* name);
