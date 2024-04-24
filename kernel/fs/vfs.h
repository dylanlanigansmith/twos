#pragma once
#include "../stdlib.h"
#include "../../common/fs.h"
/*
Let's Makeup a Filesystem

aw man turns out we wanna be somewhat POSIX adjacent
*/





extern vfs_node* vfs_root;

vfs_node* init_vfs(uintptr_t ptr, size_t size);


void vfs_printfile(vfs_node* node, size_t len);

void vfs_printfile_at(vfs_node* node,size_t offset, size_t len);
