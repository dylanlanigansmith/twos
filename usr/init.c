#include "../libd/libd.h"
#include "../libd/graphics.h"

bool update_prompt = True;
bool lshift = False;
char cmd[128] = {0};
int cmd_idx = 0;
uint64_t last_processed_input = 0;
void shell_init() {
    update_prompt = True;
    memset((void*)cmd, 0, sizeof(cmd));
    lshift = False;
    cmd_idx = 0;
    last_processed_input = 0;
}
typedef struct {
    const char *name;
    uint8_t flags; 
    void (*fn)(int argc, char* argv);
} shell_cmd_t;


void cmd_ls(int c, char* v)
{
    //printf("why don't you LS some bitches\n");

    execc("ls"); //ahhhhhh
}

void cmd_ticks(int c, char* v)
{
    printf("ticks: %li\n", sys_gettick());
}
void cmd_quit(int c, char* v)
{
     printf("quitting\n");
     sys_shutdown();
}

void cmd_testsprintf(int c, char* v)
{
   
    char a[9];
    for(int p = 0; p < 11; ++p){
        int aa = snprintf(a, 9, "STKEYS%d", p);

        printf("%i '%s' /  %i", p, a, aa);
    }
    
    for(int p = 99; p < 110; ++p){
        int aa = snprintf(a, 9, "STKEYS%d", p);

        printf("%i '%s' / %i", p, a, aa);
    }
    for(int p = 0; p < 3; ++p){
        for(int k = 5; k > 0; --k){
            
             int aa = snprintf(a, 9, "STFST%d%d", p,k);
            printf("%i '%s' / ", p, a);
        }
       
    }
   
    print("\n=====\n");
    for (int i=0; i<10; ++i)
    {
        char buf[12];

       int aa = snprintf(buf, sizeof(buf), "chatmacro%i", i);
       printf("%i '%s' \n", i, buf);
    }
     print("\n=====\n");


    char b[256] = {0};
    int i = snprintf(b, 64, "dob: %i/%i/%i name: %s sex: %s address %lx \n", 1,1,2001, "crash", "yes", (uintptr_t)b);
     char s[128] = {0};
    printf("returned %i vs strlen %li \n", i, strlen(b));
    printf("result: %s\n", b);
    i = snprintf(s, 32, "dob: %i/%i/%i name: %s sex: %s address %lx \n", 11,6,2001, "user", "maybe?", (uintptr_t)b);

    printf("returned %i vs strlen %li \n", i, strlen(s));
    printf("result: %s\n", s);


    //__asm__ volatile ("int 3;");*/
}
void cmd_doom(int c, char* v)
{
    execc("doomos");
    print("DOOM EXITED");
}

void cmd_dbg(int c, char* v)
{
    printf("tick = %li \n", sys_gettick());
    sleep_ms(3000);
    for(int i = 0; i < 6; ++i){
         printf("sleepy = %li \n", sys_gettick());
    }

    
}

void cmd_mem(int c, char* v)
{
    void* alloc =  malloc(69);
    if(!alloc){
        print("malloc failed!"); return;
    } 
     printf("malloced a little %lx", (uintptr_t)alloc);
     alloc = 0;
    alloc = malloc(PAGE_SIZE * 4);
    if(!alloc){
        print("malloc failed!"); return;
    } 


    printf("malloced a lot %lx", (uintptr_t)alloc);

    printf("now testing write/read\n");
    memset(alloc, 69, PAGE_SIZE);

    uint8_t val = *(uint8_t*)(alloc + 8000); 
    printf("[%i] == 69? \n", val);


    *(uint64_t*)(alloc + 33000) = 8008135llu;


    uint64_t vv = *(uint64_t*)(alloc + 33000);
     printf("[%li] == 8008135? \n", vv);

    print("testing mmap... ");
    void* mmwad = mmap_file("DOOM.WAD");
    print("did we do it???\n");

    printf("mmap wad [%lx] \n", (uintptr_t)mmwad);

    char* s = malloc(10);
    memcpy(s, mmwad, 5);
    s[6] = 0;
     printf("mmap wad = '%s' \n", s);

}

void cmd_crash(int c, char* v){
    *(uint64_t*)(0x000) = 69;

    
}

void cmd_cls(int c, char* v){
    syscall(21, 0);
}
void cmd_dumpmem(int c, char* v){
    syscall(20, 0);
}

void cmd_help(int c, char *v);

shell_cmd_t cmds[] =
{
    {"ls", 0, cmd_ls},
    {"ticks", 0, cmd_ticks},
    {"q", 0, cmd_quit},
    {"dbg", 0, cmd_dbg},
    {"doom", 0, cmd_doom},
    {"malloc", 0, cmd_mem},
    {"sprint", 0, cmd_testsprintf},
    {"crash", 0, cmd_crash},
    {"infomem", 0, cmd_dumpmem},
    {"cls", 0, cmd_cls},
    {"help", 0, cmd_help},
    {0,0,0}
};

void cmd_help(int c, char *v){
    printf("listing all commands!\n");
     for( int i = 0; cmds[i].name != 0; ++i){
        printf("    cmd: < '%s' >\n", cmds[i].name);
    }
    printf("hope that helped! \n");
}

int execute_cmd(const char* cmd){
    print("\r \n");

    for( int i = 0; cmds[i].name != 0; ++i){
        if(!strcmp(cmd, cmds[i].name)){
            cmds[i].fn(0,0);
            return 0;
        }
    }
    printf("unknown command: '%s' \n\0", cmd);
    return 1;
   
}

void run_shell()
{
    //this gonna be messy
    uint64_t tick = sys_gettick();
   
    if(update_prompt){
        print("\n");
        printf("usr@os> "); update_prompt = False;
    }
    uint64_t keytick =  keyboard_lastinput();
    if(keytick == last_processed_input) return;

    last_processed_input = keytick;
    uint8_t sc = keyboard_get_lastevent(); //only DOWN events
    if(sc == 0xff || !sc) return;
    if(sc & PS2_MAKEORBREAK){
        //break
        sc &= 0x7f;
        if(sc == PS2_KEY_LEFT_SHIFT)
            lshift = False;

        if(sc == PS2_KEY_ENTER && cmd_idx > 0){
            
            
            execute_cmd(cmd);
            memset((void*)cmd, 0, sizeof(cmd));
            cmd_idx = 0;

            update_prompt = True;
        }

    } else{

        if(sc == PS2_KEY_LEFT_SHIFT)
            lshift = True;
        //make
         char c = keyboard_sc2ascii(sc);
        //our old kernel level code 
        if(c != 0x0){
            if(lshift)
                c = apply_shift_modifier(c);

            if(cmd_idx >= 127) cmd_idx = 0; //dunno
            putc(c);

            if(c != '\r'){
                cmd[cmd_idx] = c;
                if(c == '\b')
                    cmd_idx = (cmd_idx > 0) ? cmd_idx - 1 : 0;
                else cmd_idx++;
            }

            
            
            

            yield();

        }
    }
   

}

uint64_t start_tick = 0;
void main(){
    //the first code we ever ran as a process in usermode
    const char* str = "I am a user task hard at work\n"; 
     const char* str2 = "please mr kernel give me cpu time\n";
    print(str);
    print(str2);
    //dont have the heart to get rid of it


    gfx_info_t* gi = gfx_get_info();
    

   
    start_tick = sys_gettick();
   shell_init();
    //print("running doom in 10 seconds");
    while (1) {
        run_shell();
        yield();
       
    }
    
    syscall(2,0);
    
}


extern void _start();
/*
void _start(){
  
    main();
    for(;;) { __asm__ volatile ("mov rax, 0xcafebabe; hlt"); }
}
*/
