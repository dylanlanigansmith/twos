#include <boot/cmdline.h>
//From randOS 4.24


struct 
{
    char args[CMDLINE_MAX_ARG][CMDLINE_MAX_ARG_LEN];

    uint32_t vals[CMDLINE_MAX_ARG];

    size_t num_args;
}cmdline;

#define ARG_SEP '='
                    //removed printing since we will use this for whether to use serial or not
#define CDEBUGF(fmt, ...) //debugf(fmt, __VA_ARGS__)

size_t cmdline_numargs()
{
    return cmdline.num_args;
}

int cmdline_parse(struct multiboot_tag_string *stag)
{
    memset(&cmdline, 0, sizeof(cmdline));
    size_t num_args = 0;

    int stag_len = stag->size - sizeof(struct multiboot_tag);
    
    if(stag_len <= 0) return 0;

    int idx = 0;
    int bdx = 0;
    char buf[64] = {0};
    while(idx < stag_len){
        if(stag->string[idx] == ARG_SEP){
            idx++; //skip equals

            buf[bdx] = 0; //should be our arg key up to the seperator + null term
            if(bdx > 16) CDEBUGF("cmd %s too long", buf);

            strncpy(cmdline.args[num_args], buf, 16);
            char sval[32]; int vdx = 0;
            while(stag->string[idx] != ' ' && stag->string[idx] != 0 && idx < stag_len){ //oh boy
                sval[vdx++] = stag->string[idx++];
            }
            if(stag->string[idx] == ' ') idx++; //skip space if thats why we broke

            sval[vdx] = 0;
           
            uint32_t val = atou(sval); 
            if(val == UINT32_MAX) { CDEBUGF("arg %s val non-numeric!\n"); cmdline.vals[num_args] = UINT32_MAX; }
            else{
                cmdline.vals[num_args] = val;
            }
            CDEBUGF("found cmdline arg %s = %u\n",cmdline.args[num_args],  cmdline.vals[num_args]);
            memset(buf, 0, 64); 
            bdx = 0;
            num_args++;
            
        }

        buf[bdx++] = stag->string[idx++];
        
    }
    cmdline.num_args = num_args;
    return num_args;
}
#define ARGIDX_NONE 0xff
size_t get_arg_idx(const char *arg)
{
    for(int i = 0; i < cmdline.num_args; ++i){
        if(arg[0] != cmdline.args[i][0]) continue;
        
        if(!strcmp(arg, cmdline.args[i])) return i;
    }

    return ARGIDX_NONE;
}

bool cmdline_hasarg(const char *arg)
{
    return (get_arg_idx(arg) != ARGIDX_NONE);
}

uint32_t cmdline_getarg(const char *arg) //returns uintmax if no arg
{
    size_t idx = get_arg_idx(arg);
    if(idx == ARGIDX_NONE) return CMDLINE_NO_ARG;
    return cmdline.vals[idx];
}

bool cmdline_is_true(const char *arg)
{
    uint32_t val = cmdline_getarg(arg);
    return (val != 0 && val != CMDLINE_NO_ARG);
}
