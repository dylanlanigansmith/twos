#include "print.h"
#include "assert.h"
#ifdef PRINT_SERIAL
#include "../../drivers/serial/serial.h"
#endif
#ifdef PRINT_GRAPHICAL
#include "../../drivers/video/gfx.h"
#include "../../drivers/video/console.h"
#endif
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

void print(const char *str)
{

#ifdef PRINT_GRAPHICAL
#ifdef VGA_MODE_GFX
    static bool gfx_ok = False;
    if(!gfx_ok)
        gfx_ok = gfx_has_init();
    else
        gfx_print(str);
#else
    console_print(str);
#endif
#endif

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


void oprintf(uint8_t out, const char* fmt, ...){
    __printf_out_fn fnprint = (out == 1) ? (__printf_out_fn)(&serial_print) : __print;
    va_list args;
    va_start(args, fmt);
    __vprintf(fnprint, fmt, 0, 0, args);

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



/*
size_t __vprintf_bad(const char *fmt, char *buf_out, size_t len, va_list args)
{
    // should be an arg
    __printf_out_fn _print = (buf_out && len) ? __sprint : __print;
    serial_println(fmt);
    // later we can check if buf is not 0 and use it vs printing
    char bufarr[PRINTF_MAX];
    char *buf = bufarr;
    int k = 0;
    size_t total = 0;
    int last_fmt_len = 0;
    for (int c = 0; fmt[c] != '\0'; ++c)
    {
        buf[k] = fmt[c];

        if (fmt[c + 1] == '%' || fmt[c + 1] == '\0')
        {

            // buf[k] = '\0';
            k = 0;

            if (buf[0] != '%')
            {
                // not a format specifier
                total += _print(buf, buf_out, len);
            }
            else
            {
                last_fmt_len = 0;
                int j = 1;
                char ch1 = 0;
                int fp_precision = 0;
                while ((ch1 = buf[j++]) < 58)
                {
                    fp_precision++;

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
                    total += _print(__PRINT_ITOA(va_arg(args, int), 16), buf_out, len);
                    last_fmt_len = 1;
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
                    printf("yeah we dont do floats yet fp: %i", fp_precision); // hell yeah
                    break;
                // Longs
                case 'l':
                case 'L': // okay so we just dont bother with longs and just use long long bc thats all its gonna be in this case but maybe ill regret this!
                    char ch2 = buf[2];
                    switch (ch2)
                    {
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
                        printf("yeah we dont do doubles yet fp: %i", fp_precision); // hell yeah
                        break;
                    case 'u':
                    case 'd':
                        last_fmt_len = 1;
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
            }
        }
    }
    // buf[k] = '\0';
    return total;
}
*/
// we should have a bullet proof print for errors etc

size_t __vprintf(__printf_out_fn _print, const char *fmt, char *buf_out, size_t len, va_list args)
{
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
                    total += _print(__PRINT_ITOA(va_arg(args, int), 16), buf_out, len);
                    last_fmt_len = 1;
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
                case 'L': // okay so we just dont bother with longs and just use long long bc thats all its gonna be in this case but maybe ill regret this!
                    char ch2 = fmt[j + 1];
                    switch (ch2)
                    {
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
                        last_fmt_len = 1;
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