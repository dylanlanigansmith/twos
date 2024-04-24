#include "print.h"
#include "assert.h"
#include "binary.h"
#include "stdbool.h"

#ifdef PRINT_SERIAL
#include "../../drivers/serial/serial.h"
#endif
#ifdef PRINT_GRAPHICAL
#include "../../drivers/video/gfx.h"
#include "../../drivers/video/console.h"
#endif

#include "../stdio/stdout.h"

size_t __print_stdout(const char* str){   
    return 0;
}

void print_stdout(const char* str){

    for(int c = 0; str[c] != 0; ++c){
        stdout_putchar(str[c]);
    }
}

void debug_string(const char *str)
{
    serial_println("\ni am printing!");
    serial_println(str);
    size_t len = strlen(str);
    if (!len)
        serial_println("len is 0");
    serial_printi("len is ", len);


    serial_println("chars:");
    for (int c = 0; c < len; ++c)
        serial_write(str[c]);
}
int gfx_ok = 0;

void putc(char c)
{
    if(gfx_ok){
        stdout_putchar(c);
    }

    serial_write(c);
}

void print(const char *str)
{

//#ifdef PRINT_GRAPHICAL
   

    
        if(gfx_ok == 0){
            gfx_ok = gfx_has_init();
            
           
        } if(gfx_ok) {

            
            print_stdout(str); }
      //  #ifndef FORCE_STDOUT
       //         gfx_print(str);
      //  #endif
            
  
     //   }
        
//#endif

#ifdef PRINT_SERIAL
    serial_print(str);
#endif

    // debug_string(str);
}

#define __PRINT_PRINTSTR(str) print(str)
#define __PRINT_LLTOA(num, base) lltoa(num, base)
#define __PRINT_ITOA(num, base) itoa(num, base)
#define __PRINT_HTOA(num) htoa(num)
void printh(uint64_t hex)
{
    __PRINT_PRINTSTR(__PRINT_HTOA(hex));
}

void println(const char *str)
{
    __PRINT_PRINTSTR(str);
    __PRINT_PRINTSTR("\n");
}



void printi(int i)
{
    __PRINT_PRINTSTR(__PRINT_ITOA(i, 10));
}

void printl(uint64_t ll, int base)
{
    __PRINT_PRINTSTR(__PRINT_LLTOA(ll, base));
}



size_t print_wrapper(const char *str)
{
    __PRINT_PRINTSTR(str);
    return 0;
}
__printf_out_fn __print = print_wrapper;



void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __vprintf(__print, fmt, 0, 0, args);

    va_end(args);
}
void debug(const char *str)
{
    serial_print(str);
}
void debugf(const char *fmt, ...)
{
   


    __printf_out_fn _debug = (__printf_out_fn)(&serial_print);
   // _debug("$: ",0,0);
    va_list args;
    va_start(args, fmt);
    __vprintf(_debug, fmt, 0, 0, args);
     //if we wanna force new lines always for our forgetful ass we will sacrifice perf
    size_t len = strlen(fmt);
    if(fmt[len - 1] != '\n') _debug("\n", 0,0);
    va_end(args);
}

void oprintf(uint8_t out, const char* fmt, ...){
    va_list args;
    va_start(args, fmt);
    if(out & _COM)
        __vprintf((__printf_out_fn)(&serial_print), fmt, 0, 0, args);
    if(out & _FB)
        __vprintf(__print, fmt, 0, 0, args);
    if(out & _OUT)
        __vprintf((__printf_out_fn)(&__print_stdout), fmt, 0, 0, args);
    

    va_end(args);
}

size_t sprint(const char *str, char *buf, size_t max_len)
{
    if (!buf || !max_len)
        return 0;

    size_t cur_len = strlen(buf);

    size_t add_len = strlen(str);

    size_t new_len = (cur_len + add_len);
    if (new_len > max_len)
    {
        char *new_str = (char *)str; // sorry
        if (add_len < (new_len - max_len - 1))
            return 0;

        // this is likely bugged and maybe not even worth it
        size_t chopped_len = add_len - (new_len - max_len - 1);
        new_str[chopped_len] = 0;

        new_len = cur_len + chopped_len;
    }

    return (strcat(buf, str) != 0) ? new_len - cur_len : 0;
}



__printf_out_fn __sprint = sprint;
size_t sprintf(char *buf, size_t len, const char *fmt, ...)
{

    va_list args;
    va_start(args, fmt);
    size_t ret = __vprintf(__sprint, fmt, buf, len, args);

    va_end(args);

    return ret;
}



size_t __vprintf(__printf_out_fn _print, const char *fmt, char *buf_out, size_t len, va_list args)
{
    
    ASSERT((uintptr_t)_print != nullptr); 
    // should be an arg
    
    // later we can check if buf is not 0 and use it vs printing
    char buf[PRINTF_MAX];
    
   
    size_t total = 0;
    int c = 0;
    int k = 0;
    while(fmt[c] != 0) {
        if(fmt[c] != '%' ){
            buf[k] =  fmt[c];
            k++;
            buf[k] = '\0';
            
        }
        
        else{
            buf[k] = '\0';
            // not a format specifier
            total += _print(buf, buf_out, len);
           
            int last_fmt_len = 0;
            int j = c;
            k = 0;
            memset(buf, 0, strlen(fmt));
            char ch1 = 0;
            int fp_precision = 0;
        
            while ((ch1 = fmt[j]) < 58)
            {
                
                fp_precision++; //this doesnt work
                j++;
                // 57 = 9 in ascii
            } // loop used for like %2f vs just %i
            // tldr it gets us to the format char in ch1
            switch (ch1)
            {
                case 'i':
                case 'd':
                case 'u':
                    // Integers:
                    total += _print(__PRINT_ITOA(va_arg(args, int), 10), buf_out, len);
                    last_fmt_len = 1;
                    break;
                case 'x':
                    // Int but Hex
                    total += _print("0x", buf_out, len); //make behaviour consistent with our messy HTOA and LLTOA funcs for 32 bit integers
                    total += _print(__PRINT_ITOA(va_arg(args, int), 16), buf_out, len);
                    last_fmt_len = 1;
                    break;
                case 'B': //stoopid temp for 8 bit override 
                case 'b':
                case 'C': //dont add extra 0s (good for flags, bad for like examining entire entries (pages))
                        last_fmt_len = 1;
                        _print("0b", buf_out, len);
                        uint32_t num  = va_arg(args, int);
                        if(num == 0){ _print("0", buf_out, len); break;}
                        int highest_bit = (ch1=='C') ? (get_highest_bit(num) * 8) : (sizeof(num) * 8 - 1);
                        highest_bit = (ch1 == 'B') ? 7 : highest_bit;
                        bool space = (ch1 != 'B');
                       // ASSERT(highest_bit <= (sizeof(num) * 8));
                        for (int i = highest_bit; i >= 0; i--) { // * 8 - 1; shouldnt need - 1 if highest_bit works
                                if ((num >> i) & 1)
                                    _print("1", buf_out, len);
                                else
                                     _print("0", buf_out, len);

                                if (i % 4 == 0 && i != highest_bit && space)
                                     _print(" ", buf_out, len); //0000 0000 0001 make head hurt less than 000001010101010
                            }
                        break;
                case 'c':
                    // Characters
                    char cb[2] = {va_arg(args, int), 0};
                    total += _print(cb, buf_out, len);
                    last_fmt_len = 1;
                    break;
                // Floats
                case 'f':
                    total += _print("no float impl", buf_out, len);
                    last_fmt_len = 1 + fp_precision;
                   // printf("fp: %i", fp_precision); // hell yeah
                    break;
                // Longs
                case 'l':
                case 'L':
                 // okay so we just dont bother with longs and just use long long bc thats all its gonna be in this case but maybe ill regret this!
                    char ch2 = fmt[j + 1];
                    switch (ch2)
                    {
                    case 'b':
                    case 'B':
                         last_fmt_len = 2;
                        _print("0b", buf_out, len);
                        uint64_t num  = va_arg(args, unsigned long long);
                        if(num == 0){ _print("0000", buf_out, len); break;}
                        for (int i = sizeof(num) * 8 - 1; i >= 0; i--) {
                                if ((num >> i) & 1)
                                    _print("1", buf_out, len);
                                else
                                     _print("0", buf_out, len);

                                if (i % 4 == 0)
                                     _print(" ", buf_out, len);;  // Add a space for better readability
                            }
                       
                        break;
                    case 'x': // btw this will prefix with 0x by default!
                        total += _print(__PRINT_HTOA(va_arg(args, unsigned long long)), buf_out, len);
                        last_fmt_len = 2;

                        break;
                    case 'X': // out of spec but this doesnt prefix with 0x
                        total += _print(__PRINT_LLTOA(va_arg(args, unsigned long long), 16), buf_out, len);
                        last_fmt_len = 2;
                        break;
                    case 'f': // double, long double
                        total += _print("no double impl", buf_out, len);
                        last_fmt_len = 2 + fp_precision;
                       // printf("fp: %i", fp_precision); // hell yeah
                        break;
                    case 'u':
                    case 'd':
                    case 'i':
                        last_fmt_len = 1; //since default probs means was no ch2 we let it inc for these cases,
                                                    // where we want default case of printing as decimal long
                    default:
                        total += _print(__PRINT_LLTOA(va_arg(args, unsigned long long), 10), buf_out, len);
                        last_fmt_len++;
                        break;
                    }
                    break;
                    
                case 's':
                    total += _print(va_arg(args, char *), buf_out, len);
                    last_fmt_len = 1;
                    break;
                default:
                    total += _print(buf, buf_out, len);
                    last_fmt_len = 1;
            }
             
             
            c += last_fmt_len; //and our normal inc accounts for %
           //  serial_printi("before ++ c = ", c);
        }
       ///if(fmt[c + 1] == 0)
        
        
        c++;

         if ( fmt[c] == 0)
        {
            buf[k] = '\0';
            total += _print(buf, buf_out, len);
           
        }  
        
    }
    
    return total;
}