#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"

typedef size_t (*__printf_out_fn)(const char *, char *, size_t);
size_t __vprintf(__printf_out_fn _print, const char *fmt, char *buf_out, size_t len, va_list args);

size_t __print_stub(const char *str)
{
    print(str);
    return 0;
}
__printf_out_fn __print = __print_stub;

void printf2(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    __vprintf(__print, fmt, 0, 0, args);
    va_end(args);
}

void printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    char b[PRINTF_BUF];
    vsnprintf(b, PRINTF_BUF, fmt, args);
  //  __vprintf(__print, fmt, 0, 0, args);
    print(b);
    va_end(args);
}

void vprintf(const char* fmt, va_list ap){
     __vprintf(__print, fmt, 0, 0, ap);
}


size_t __sprint(const char *str, char *buf, size_t max_len)
{
    if (!buf || !max_len)
        return 0;

    size_t cur_len = strlen(buf);

    size_t add_len = strlen(str);
    /*if (new_len > max_len && 0) //this actually worked well
    {
        char *new_str = (char *)str; // sorry
        if (add_len < (new_len - max_len - 1))
            return 0;

        // this is likely bugged and maybe not even worth it
        size_t chopped_len = add_len - (new_len - max_len - 1);
        new_str[chopped_len] = 0;

        new_len = cur_len + chopped_len;
    }*/ 

    //just cheat and use strncat
    size_t max_add = (max_len > cur_len) ? (max_len - cur_len) : 0; //delta between max and cur size

    max_add = (max_add > add_len) ? add_len : max_add; //are we adding less than our existing room ?
    char* cat = strncat(buf, str, max_add ) ;

    size_t catlen = strlen(cat); //once this fn is working just optimize this 


    return (catlen != 0) ? catlen - cur_len : 0;
}



__printf_out_fn __sprintfn= __sprint;



int snprintf(char *s, size_t n, const char * fmt, ...)
{
    memset(s, 0, n);
    va_list ap;
	va_start(ap, fmt);
    size_t ret = __vprintf(__sprintfn, fmt, s, n, ap);
	va_end(ap);

   // s[n - 1] = 0; //s[n] ?
	return (int)ret;
    
}
int vsnprintf(char *s, size_t n, const char * fmt, va_list args)
{
    memset(s, 0, n);
   
    size_t ret = __vprintf(__sprintfn, fmt, s, n, args);


   // s[n - 1] = 0; //s[n] ?
	return (int)ret;
    
}
#define __PRINT_LLTOA(num, base, buf) lltoa(num, buf, base)
#define __PRINT_ITOA(num, base, buf) itoa(num, buf, base)
#define __PRINT_HTOA(num, buf) lltoa(num, buf, 16)






size_t __vprintf(__printf_out_fn _print, const char *fmt, char *buf_out, size_t len, va_list args)
{
    
    ASSERT((uintptr_t)_print != nullptr); 
    // should be an arg
    
    // later we can check if buf is not 0 and use it vs printing
    char buf[PRINTF_BUF];
    
   
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
            char ub[32] = {0};
            memset(ub, 0, 32);
            switch (ch1)
            {
                case 'i':
                case 'd':
                case 'u':
                    // Integers:
                    
                    __PRINT_ITOA(va_arg(args, int), 10, ub);
                    total += _print(ub, buf_out, len);
                    last_fmt_len = 1;
                    break;
                case 'x':
                    // Int but Hex
                    total += _print("0x", buf_out, len); //make behaviour consistent with our messy HTOA and LLTOA funcs for 32 bit integers
                    
                    __PRINT_ITOA(va_arg(args, int), 16, ub);
                    total += _print(ub, buf_out, len);
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
                    case 'X': // out of spec but this doesnt prefix with 0x
                         
                        __PRINT_LLTOA(va_arg(args, int), 16, ub);
                        total += _print(ub, buf_out, len);
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
                        
                        __PRINT_LLTOA(va_arg(args, int), 10, ub);
                        total += _print(ub, buf_out, len);
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

