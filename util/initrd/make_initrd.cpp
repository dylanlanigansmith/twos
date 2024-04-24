#include <iostream>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <filesystem>
#include <vector>
#include <unordered_map>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h> //i get to write c++ for first time in a while, and yet here we end up 
#include <unistd.h>
#include "../../kernel/fs/initrd_spec.h"

namespace fs = std::filesystem; //sorry its so long

/*
./make_initrd [./rootdir/] [outfile]
This bad boy takes the folder at arg 1: rootdir, and maps whatever directories and files are in it into our 
    proprietary ram disk format, writing it to arg2; outfile

Then with the ram disk in the boot iso it is loaded by multiboot2 and able to be mounted within the OS


Issues:
    -not hashing the full paths for directory processing, so like: "example/example/example.txt" would cause issues
        - easy fix once the OS side of things is working, not important now

*/


int main(int argc, char** argv){

    if(argc != 3){
        puts("usage: ./make_initrd [./rootdir/] [outfile] \n"); return 1;
    }

    //this is not gonna be good code FYI
    //read on if you dare


    const std::string rootdir = argv[1];
    const std::string out_name = argv[2];
    printf("making initrd file %s from dir %s \n", rootdir.c_str(), out_name.c_str());

    /*
    Test folder has 10mb files, binary files, and a few novels
    std::filesystem dont fail me now
    
    Steps to make initrd

    sum up num files, num dirs

    make header 

    itr dirs, making dir info entries at each level
        - need to handle parents 
        -either hashmap and LUT or go one level at a time 

    
    
    */
    initrd_header header = {
        0, 0,  INITRD_ROOT, INITRD_CRC
    };
    uint32_t num_files =0, num_dirs = 0;

    fs::path root = fs::absolute(rootdir);
    std::cout << root << std::endl;

    for (const auto& entry : fs::recursive_directory_iterator(rootdir)){
        
      //  std::cout << entry << std::endl;
        if(entry.is_directory()){
           // printf("found dir %s %i\n", entry.path().filename().c_str(), num_dirs + 1);
            num_dirs++; continue;
        }
        else{
            num_files++;
        }

        
    }
    header.num_dirs = num_dirs;
    header.num_files = num_files;
  
    
    
    printf("\n Found %u files and %u directories \n", num_files, num_dirs);

    struct dirinfo{
        uint32_t inode;
        uint32_t parent_node;
        std::string parent;
    };
    
    std::unordered_map<std::string, dirinfo> dirs =  {   {std::string("root"),{0,0,std::string()} }    } ;
          
    
    
    
     //0
     uint32_t curdir_node = 1;
    
    for (auto& entry : fs::recursive_directory_iterator(rootdir)){
        if(!entry.is_directory()) continue;
        auto path = entry.path();
        std::string parent_name =  std::string(path.parent_path().filename());
        uint32_t parent_node = dirs[parent_name].inode;
        //WARNING:
        // we really should insert via full path into map so that root/example/example/example.txt doesnt break it all
        //future problem tho Min Viable Product
        dirs.insert({path.filename(), {
            curdir_node++,
            parent_node,
            parent_name
            }
            
            });
    }
    
   


   


    for (auto& entry : dirs){
        printf("Added Directory: '%s' I[%u],  Parent [%u] '%s' \n", entry.first.c_str(), entry.second.inode, entry.second.parent_node, entry.second.parent.c_str());
    }
    /*
     so we now have everything we need to make directory info sections and initrd header, lets do files! 
    */
   
   std::vector< std::pair< initrd_file_header, fs::path >  > files;
    size_t files_total = 0;
    for (auto& entry : fs::recursive_directory_iterator(rootdir)){
        if(entry.is_directory()) continue;
        auto path = entry.path();

        std::string parent_dir_name =  std::string(path.parent_path().filename());

        initrd_file_header head ={
            .name = 0,
            .offset = 0,
            .size = entry.file_size(),
            .parent_dir =  dirs[parent_dir_name].inode,
            .perms = 0b00001110, //RWX but probs doesnt matter
            ._zero = 0,
            .crc = INITRD_CRC,
            
        };

        files_total += head.size;
        assert( std::string(path.filename()).length() < 128);
        strcpy((char*)head.name, path.filename().c_str());

        files.push_back({head, path});
    }

    for(const auto& file : files){
        printf("Added File: '%s', size %lu, parentdir[%u], crc = %x \n", file.first.name,  file.first.size, file.first.parent_dir, file.first.crc);
    }
    printf("Total Size of Files in InitRd:  %lu bytes  [%lu kb] \n", files_total, files_total / 1024);
    
    //okay now we are ready to write some bytes
    
     if(!dirs.erase("root")) puts("failed to delete rootdir what \n"); // can get rid of this now 


    //final checks
    printf("dir size %lu vs %u \n", dirs.size(), header.num_dirs );
    
    assert(files.size() == (size_t)header.num_files);
    assert(dirs.size() == (size_t)header.num_dirs);

    puts("writing initrd \n");
    std::ofstream out;
    out.open(out_name, std::ios::binary | std::ios::out);
    if(out.fail() ){
        puts("failed to open output file! \n"); return 1;
    }
    size_t total = 0;

    //this is where the bad code comes out
  


    out.write((char*)&header, sizeof(header)); //write header
    total += sizeof(header);
    assert(!out.bad()); 
    printf("wrote header! current total = %lu \n", total);
    for(const auto& dir : dirs){
        initrd_dir_info info ={
            .parent_inode = dir.second.parent_node,
            .dir_inode = dir.second.inode,
            .name = 0,
            .crc = INITRD_CRC
        };
        assert( dir.first.length() < 128);
        strcpy((char*)info.name,  dir.first.c_str());

        out.write((char*)&info, sizeof(info));
        assert(!out.bad()); 
        total += sizeof(info);
    }
    printf("wrote dirs! current total = %lu \n", total);

    //okay THIS is where it gets bad

    for(auto& head : files){
        head.first.offset = total; //probs fine
        out.write((char*)&head.first, sizeof(head.first));
        assert(!out.bad()); 
        total += sizeof(head.first);
        /*
        std::ifstream in(head.second, std::ios::binary | std::ios::in);
        if(in.fail()){
            printf("failed to open %s \n", head.second.c_str()); return 1;
        } */

        //fuck a buffer 
        struct stat s;
        stat(head.second.c_str(), &s);
        size_t file_size = s.st_size;
        assert(file_size == head.first.size); //for fun
        int fd = open(head.second.c_str(), O_RDONLY);
        char* buf = (char*)mmap(0, file_size, PROT_READ, MAP_FILE|MAP_PRIVATE, fd, 0);
        if(!buf || buf == MAP_FAILED){
            close(fd);
            printf("failed to mmap %s \n", head.second.c_str()); return 1;
        }
        out.write(buf, file_size);
        assert(!out.bad()); 
        total += file_size;
        printf("wrote file %s, current total = %lu \n",head.first.name, total);
        if(munmap(buf, file_size) != 0){
            puts("failed to unmmap \n");
        }
        close(fd);

        //oh boy
    }

    printf("wrote files! current total = %lu \n", total);

    initrd_footer foot = {
        .size = total,
        .crc = INITRD_CRC
    };
    out.write((char*)&foot, sizeof(foot));
    assert(!out.bad()); 
    total += sizeof(foot);
    printf("wrote footer! total bytes written = %lu bytes, [%lu kb] \n", total, total / 1024);
    out.close();
    if(out.fail()){
        puts("failed to close file! \n"); return 1;
    }
    puts("created initrd! \n");

    printf("note: %s has a size of %lu, expected %lu \n", out_name.c_str(), fs::file_size(fs::path(out_name)), total);
    return 0;
}