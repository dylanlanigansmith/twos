#include "../libd/libd.h"


bool update_prompt = True;
bool lshift = False;
char cmd[128] = {0};
int cmd_idx = 0;
uint64_t last_processed_input = 0;
void shell_init() {
    update_prompt = True;
    memset((void*)cmd, 0, sizeof(cmd));
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
void cmd_dbg(int c, char* v)
{
    __asm__ volatile ("int 3;");
}
void cmd_doom(int c, char* v)
{
    execc("doomos");
}

shell_cmd_t cmds[] =
{
    {"ls", 0, cmd_ls},
    {"ticks", 0, cmd_ticks},
    {"q", 0, cmd_quit},
    {"dbg", 0, cmd_dbg},
    {"doom", 0, cmd_doom}
};


int execute_cmd(const char* cmd){
    print("\r \n");

    for( int i = 0; cmds[i].name != 0; ++i){
        if(!strcmp(cmd, cmds[i].name)){
            cmds[i].fn(0,0);
            return 0;
        }
    }
    printf("unknown command: %s\n", cmd);
    return 1;
   
}

void run_shell()
{
    //this gonna be messy
    uint64_t tick = sys_gettick();
   
    if(update_prompt){
        
        printf("\n%li$>>> ", tick); update_prompt = False;
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
                    cmd_idx--;
                else cmd_idx++;
            }

            
            
            

            yield();

        }
    }
   

}


void main(){
    const char* str = "I am a user task hard at work\n";
     const char* str2 = "please mr kernel give me cpu time\n";
    print(str);
 
    print(str2);
   

   size_t l = strlen(str);
   char lol[300] = {0};
   memset((void*)lol, 'A', 300);

   
    
    uint64_t i = 0;
    while (1) {
        run_shell();

        //yield();
    }
    
    syscall(2,0);
    
}

void _start(){
  
    main();
    for(;;) { __asm__ volatile ("mov rax, 0xcafebabe; hlt"); }
}
