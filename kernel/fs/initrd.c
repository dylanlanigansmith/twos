
#include "initrd.h"
#include "../mem/heap.h"

initrd_header* header;
initrd_footer* footer;

size_t initrd_read(vfs_node* node,  uint64_t offset, size_t size, uint8_t* buf){

    return 0;
}
size_t initrd_write(vfs_node* node,  uint64_t offset, size_t size, uint8_t* buf){

    return 0;
}

void initrd_open(vfs_node* node){}
void initrd_close(vfs_node* node){ }

vfs_node* initrd_readdir(vfs_node* node, uint32_t inode){
    
    return 0;
}
void* initrd_finddir(vfs_node* node, const char* name){

    return 0;
}

size_t initrd_nodes_size = 0;
vfs_node* initrd_root = 0;
vfs_node* initrd_nodes = 0;
uint32_t initrd_last_inode = 0;


vfs_node* initrd_find_root(uint32_t inode)
{
    debugf("looking for node %i", inode);
    for(int i = 1; i < initrd_nodes_size; ++i){
        debugf("findroot %i \n", i);
        vfs_node* node = &initrd_nodes[i];

        if(node->name[0] == 0) continue;
        debugf("looking at %s \n", node->name);
        if(node->inode == inode)
            return node;
    }

    return 0;
}

vfs_node *initrd_findfile(vfs_node *root, const char *name) //also finds dirs
    //dont talk to me about this function it came from a dark place
{
    
     for(int i = 0; i < initrd_nodes_size; ++i){
        vfs_node* node = &initrd_nodes[i];

        if(node->name[0] == 0) continue;
       // debugf("looking at %s \n", node->name);
        if(!strcmp(node->name, name))
            return node;
    }

    return 0;
    
    return nullptr;
}

vfs_node* initrd_find_node(uint32_t inode)
{
    
    for(int i = 0; i < initrd_nodes_size; ++i){
        vfs_node* node = &initrd_nodes[i];

        if(node->name[0] == 0) continue;
       // debugf("looking at %s \n", node->name);
        if(node->inode == inode)
            return node;
    }

    return 0;
}

vfs_node* initrd_find_dir_by_node(uint32_t inode) //redundant 
{
    for(int i = 0; i < initrd_nodes_size; ++i){
        vfs_node* node = &initrd_nodes[i];
        if(!node->perm.D) continue;

        if(node->name[0] == 0) continue;

        if(node->inode == inode)
            return node;
    }

    return 0;
}

void initrd_add_root_node(vfs_node* node, vfs_dir_info* dir_info)
{
    node->type = VFS_ROOT;
    node->perm.D = 1;
    node->perm.R = 1;
    node->perm.W = node->perm.X = node->perm.lvl = 0 ;
    node->impl = VFS_IMPL_INITRD;
    node->length = 0; 
    strcpy(node->name, "initrd");
    
    node->fn.open = initrd_open;
    node->fn.close = initrd_close;
    node->fn.readdir = initrd_readdir;
    node->fn.finddir = initrd_finddir; 
    

    
    dir_info->dir_inode =node->inode = 0; //wtf
    dir_info->parent_inode = 0;
    dir_info->parent = 0;
    dir_info->size = 0;
    node->dirinfo = dir_info;

    initrd_nodes_size++;
}

void initrd_add_dir_node(vfs_node* node, initrd_dir_info* dir, vfs_dir_info* dir_info)
{
    node->type = VFS_DIR;
    node->perm.D = 1;
    node->perm.R = 1;
    node->perm.W = node->perm.X = node->perm.lvl = 0 ;
    node->impl = VFS_IMPL_INITRD;
    node->length = 0; 
    strcpy(node->name, dir->name);

    node->fn.open = initrd_open;
    node->fn.close = initrd_close;
    node->fn.readdir = initrd_readdir;
    node->fn.finddir = initrd_finddir; 
    

    node->inode = dir->dir_inode; //wtf
    dir_info->dir_inode = dir->dir_inode;
    dir_info->parent_inode = dir->parent_inode;
    dir_info->parent = (dir->parent_inode == 0) ? initrd_root : 0; //dumb just do it all at once
    node->dirinfo = dir_info;
    dir_info->size = 0;
    initrd_nodes_size++;
    debugf("added dir node %s \n", node->name);
}



void initrd_add_file_node(vfs_node* node, initrd_file_header* file, uint32_t inode){

    node->inode = inode; //cross fingers 
    node->type = VFS_FILE;
    node->perm.X = 1;
    node->perm.R = 1;
    node->perm.W = node->perm.D = node->perm.lvl = 0 ; //initrd is RO
    node->impl = VFS_IMPL_INITRD;
    
    strcpy(node->name, file->name);

    node->length = file->size; 
    node->addr =  (uintptr_t)(file) + sizeof(initrd_file_header); //should just fix these when we first index them!!!


    node->fn.open = initrd_open;
    node->fn.close = initrd_close;
    node->fn.read = initrd_read;
    node->fn.write = initrd_write;

    vfs_node* parent = initrd_find_dir_by_node(file->parent_dir);
    if(!parent){
        debugf("failed to find a parent folder for file %s?\n", node->name ); KPANIC("sorry i didn't know what else to do");
    }

    parent->dirinfo->entries[parent->dirinfo->size++] = node->inode;
    ASSERT(parent->dirinfo->size < VFS_DIR_MAX_ENTRIES); //not a fan of this

    node->p = parent; //why not!

    initrd_nodes_size++;
    debugf("added file node %s \n", node->name);
}


int initrd(uintptr_t addr, size_t size)
{
    //TODO: check if addr has virtual/id map!!!!!
        //bochs also loaded at 0x123000 so multiboot might be blessin us up
    header = footer = 0;
    if (!addr || ! size){
        debugf("initrd: failed! invalid address, invalid size"); return 1;
    }   

    debugf("===initrd: loading %lu bytes from %lx===\n", size, addr);

    header = (initrd_header*)(addr);

    if (header->crc != INITRD_CRC){ debugf("initrd: header CRC INVALID! \n"); return 1; }
    debugf("initrd: header CRC ok! \n");
    debugf("initrd: header:\n %u files, %u dirs\n", header->num_files, header->num_dirs);

    footer = (initrd_footer*)(addr + size - sizeof(initrd_footer));
    if (footer->crc != INITRD_CRC){ debugf("initrd: footer CRC INVALID! \n"); return 1; }
    debugf("initrd: footer CRC ok! \n");
    debugf("initrd: footer: total ramdisk size = %lu \n", footer->size); //redundant but makes me happy

    initrd_dir_info* dirs = (initrd_dir_info*)(  addr + sizeof(initrd_header) );
    debugf("initrd: processing directories:\n");
    for(int i = 0; i < header->num_dirs; ++i){
        printf("    found dir: %s [%u] CRC=%i\n", dirs[i].name, dirs[i].dir_inode, (dirs[i].crc == INITRD_CRC));
    }

    initrd_file_header* files[header->num_files];
    size_t offset = addr + sizeof(initrd_header) + (sizeof(initrd_dir_info) * header->num_dirs);
    
    size_t total_file_size = 0;
    debugf("initrd: processing files:\n");
    for(int i = 0; i < header->num_files; ++i){
        files[i] = (initrd_file_header*)(offset);
        if(files[i]->crc != INITRD_CRC) { debugf("initrd: fileheader[%i] CRC INVALID! \n", i); return 1; }
        
        debugf("    found file at +%lx! %s [%lu bytes], dir[%u]\n", files[i]->offset, files[i]->name, files[i]->size, files[i]->parent_dir);
        total_file_size += files[i]->size;
        offset += (sizeof(initrd_file_header) + files[i]->size);
    }

    debugf("initrd validated; contains %lu bytes of files!", total_file_size);
   
    //mount as root:

    //this all will have to be refactored
    initrd_nodes_size = header->num_dirs + header->num_files + 2;
    initrd_nodes = kmalloc( sizeof(vfs_node) * (initrd_nodes_size)   ); //all our dirs, files, and +1 for root node

    memset(initrd_nodes, 0,  sizeof(vfs_node) * (initrd_nodes_size));

    if(initrd_nodes == KMALLOC_FAIL) { debugf("initrd: failed to allocate space for vfs\n"); return 1; }
    
   
    vfs_dir_info* dir_info = kmalloc(sizeof(vfs_dir_info) * (header->num_dirs + 1)); //this isnt ideal btw
    if(dir_info == KMALLOC_FAIL) { debugf("initrd: failed to allocate space for dirinfo\n"); return 1; }
    memset(dir_info, 0,  sizeof(vfs_dir_info) * (header->num_dirs + 1)) ; //+1 for root node
    
    initrd_nodes_size = 0;
    initrd_root = &initrd_nodes[0];
    
    initrd_add_root_node(initrd_root, dir_info);

    
    /*
        so we could sort by inode here first, not great but works and files will be in order anyways 
        fix = you shoulda done/can do an ordered map in initrd contruction tool, bada bing bada boom
    */
    
    int node_idx = 1;
    for(int i = 0; i < header->num_dirs; ++i){

        vfs_dir_info* di = (vfs_dir_info*)((uintptr_t)dir_info + (node_idx * sizeof(*di))  );
        initrd_add_dir_node(&initrd_nodes[node_idx++], &dirs[i], di);
    }
                    //header->num_dirs + 1
    for(int i = 1; i < (node_idx); ++i ) { //<= bc root node at 0
        //brute force find and fill subdirs

        vfs_node* dir = &initrd_nodes[i];
        
        vfs_node* parent = initrd_find_dir_by_node(dir->dirinfo->parent_inode); 

        if(!parent) {
            debugf("couldnt find parent node %u for dir %s \n", dir->dirinfo->parent_inode, dir->name); return 1;
        }
        //found parent, so we gotta add ourselves to their dirinfo entries
        dir->dirinfo->parent = parent;
        parent->dirinfo->entries[parent->dirinfo->size++] = dir->inode;
        ASSERT(parent->dirinfo->size < VFS_DIR_MAX_ENTRIES); //fun day when this happens 
    }


    for(int i = 0; i < header->num_files; ++i){
        initrd_add_file_node(&initrd_nodes[node_idx], files[i], node_idx);
        node_idx++; //gonna stray away from undef. behaviour here at the sake of looking less fancy
    }

    //add all files to dirs 






    debugf("==initrd load success!== \n\n");
    return 0;
}


void itr_dir(vfs_node* dir, int indent){ //you gotta love a recursive function 
    for(int i = 0; i < dir->dirinfo->size; ++i){
        vfs_node* node = initrd_find_node(dir->dirinfo->entries[i]);
        if(!node) continue; //whaaat

        for(int in = 0; in < indent; ++in)  
            print("    ");
        
        printf((node->perm.D) ? "%s/\n" : "%s\n", node->name);
      
        if(node->type == VFS_FILE) continue;
            
        if(node->dirinfo->size > 0){
            itr_dir(node, indent + 1);
        }
        
    }
}

void initrd_demo()
{
    debugf("initrd demo: expanding fs: \n");
    printf("\ninitrd/\n");

    print("\n");
    itr_dir(initrd_root, 1); 
    print("\n");
    debugf("conclude initrd demo i hope you are impressed\n");


}
