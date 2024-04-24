#include "stdio.h"
#include "string.h"
#include "os.h"



static int get_highest_bit(uint32_t num) { //32 
    if (num == 0) return 0; 
    int position = sizeof(num) * 8 - 1; 
    while (!(num & (1u << position))) {
        position--; 
    }
    return position;
}
void putc(char c){
    uint64_t ch = c;
     __asm__ volatile ("mov rdi, %0; mov rax, 5; int 0x69; " :: "r"(ch) : "rdi", "rax") ;
}

void printf(const char *fmt, ...) //this is just chopped from kernel for now we can fix it later 
{
    #define _print(str) print(str)
    #define __PRINT_LLTOA(num, base) lltoa(num, base)
    #define __PRINT_ITOA(num, base) itoa(num, base)
    #define __PRINT_HTOA(num) htoa(num)
    va_list args;
    va_start(args, fmt);
    
    // later we can check if buf is not 0 and use it vs printing
    char buf[PRINTF_BUF];
    
   
    size_t total = 0;
    int c = 0;
    int k = 0;

    //god this impl is bad
    //rewrite STAT now that we are in usermode
    //for one it should buffer prints till the end then do 1 syscall
    //for 2 should be like vfprintf and stdout should be a stream
    // for 3 see pt 1 and then just look at it its fucked
    while(fmt[c] != 0) {
        if(fmt[c] != '%' ){
            buf[k] =  fmt[c];
            k++;
            buf[k] = '\0';
            
        }
        
        else{
            buf[k] = '\0';
            // not a format specifier
            _print(buf);
           
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
                    _print(__PRINT_ITOA(va_arg(args, int), 10));
                    last_fmt_len = 1;
                    break;
                case 'x':
                    // Int but Hex
                    _print("0x"); //make behaviour consistent with our messy HTOA and LLTOA funcs for 32 bit integers
                    _print(__PRINT_ITOA(va_arg(args, int), 16));
                    last_fmt_len = 1;
                    break;
                case 'B': //stoopid temp for 8 bit override 
                case 'b':
                case 'C': //dont add extra 0s (good for flags, bad for like examining entire entries (pages))
                        last_fmt_len = 1;
                        _print("0b");
                        uint32_t num  = va_arg(args, int);
                        if(num == 0){ _print("0"); break;}
                        int highest_bit = (ch1=='C') ? (get_highest_bit(num) * 8) : (sizeof(num) * 8 - 1);
                        highest_bit = (ch1 == 'B') ? 7 : highest_bit;
                        bool space = (ch1 != 'B');
                       // ASSERT(highest_bit <= (sizeof(num) * 8));
                        for (int i = highest_bit; i >= 0; i--) { // * 8 - 1; shouldnt need - 1 if highest_bit works
                                if ((num >> i) & 1)
                                    _print("1");
                                else
                                     _print("0");

                                if (i % 4 == 0 && i != highest_bit && space)
                                     _print(" "); //0000 0000 0001 make head hurt less than 000001010101010
                            }
                        break;
                case 'c':
                    // Characters
                    char cb[2] = {va_arg(args, int), 0};
                    _print(cb);
                    last_fmt_len = 1;
                    break;
                // Floats
                case 'f':
                    _print("no float impl");
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
                        _print("0b");
                        uint64_t num  = va_arg(args, unsigned long long);
                        if(num == 0){ _print("0000"); break;}
                        for (int i = sizeof(num) * 8 - 1; i >= 0; i--) {
                                if ((num >> i) & 1)
                                    _print("1");
                                else
                                     _print("0");

                                if (i % 4 == 0)
                                     _print(" ");;  // Add a space for better readability
                            }
                       
                        break;
                    case 'x': // btw this will prefix with 0x by default!
                        _print(__PRINT_HTOA(va_arg(args, unsigned long long)));
                        last_fmt_len = 2;

                        break;
                    case 'X': // out of spec but this doesnt prefix with 0x
                        _print(__PRINT_LLTOA(va_arg(args, unsigned long long), 16));
                        last_fmt_len = 2;
                        break;
                    case 'f': // double, long double
                        _print("no double impl");
                        last_fmt_len = 2 + fp_precision;
                       // printf("fp: %i", fp_precision); // hell yeah
                        break;
                    case 'u':
                    case 'd':
                    case 'i':
                        last_fmt_len = 1; //since default probs means was no ch2 we let it inc for these cases,
                                                    // where we want default case of printing as decimal long
                    default:
                        _print(__PRINT_LLTOA(va_arg(args, unsigned long long), 10));
                        last_fmt_len++;
                        break;
                    }
                    break;
                    
                case 's':
                    _print(va_arg(args, char *));
                    last_fmt_len = 1;
                    break;
                default:
                    _print(buf);
                    last_fmt_len = 1;
            }
             
             
            c += last_fmt_len; //and our normal inc accounts for %
           //  serial_printi("before ++ c = ", c);
        }
       
        
        
        c++;

        if ( fmt[c] == 0)
        {
            buf[k] = '\0';
            _print(buf);
           
        }  
        
    }
    
    va_end(args);
}

void puts(const char *str)
{
         print(str);
}

int snprintf(char *restrict s, size_t n, const char *restrict fmt, ...)
{
    int ret = 0;
    va_list ap;
	va_start(ap, fmt);
	//vsnprintf
	va_end(ap);
	return ret;
    
}

int sscanf(const char *str, const char *fmt, ...){
    int ret = 0;
    va_list ap;
	va_start(ap, fmt);
	//vsnprintf
	va_end(ap);
	return ret;
}   

int fprintf(FILE *restrict f, const char *restrict fmt, ...)
{
	int ret;
	va_list ap;
	va_start(ap, fmt);
	ret = 0;//vfprintf(f, fmt, ap);
	va_end(ap);
	return ret;
}

int vfprintf(FILE *restrict f, const char *restrict fmt, va_list ap)
{
    return 0;
}

int vsnprintf(char *restrict s, size_t n, const char *restrict fmt, va_list ap){
    return 0;
}