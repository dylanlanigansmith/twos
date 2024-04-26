#include "../libd/libd.h"
//ugh
#include "../common/fs.h"



int main(int argc, char* argv)
{
    //print("why dont you LS some bitches");
    syscall(14,0);
    return 0;
}


/*void _start(){
  
    int err = main(0,0);
    exit(err);
}*/