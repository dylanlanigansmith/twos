#include "string.h"
#include "memory.h"
#include "print.h"
//from: https://www.strudel.org.uk/itoa/
char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	//weird bug
	if(val == 0){
		buf[0] = '0';
		buf[1] = 0;
		return &buf[0];
	}

	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}
char* lltoa(uint64_t val, int base){
	
	static char buf[62] = {0};
	
	//weird bug
	if(val == 0){
		buf[0] = '0';
		buf[1] = 0;
		return &buf[0];
	}

	int i = 60;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}
char* htoa(uint64_t val)
{
	const char prefix[2] = {"0x"};
	static char buf[64] = {0};
	buf[0] = prefix[0];
	buf[1] = prefix[1];
	buf[2] = 0;

	
	return strcat(buf, lltoa(val, 16));
}

uint32_t atou(const char *str) //returns max uint32 on failure
{
    uint32_t result = 0;
   
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] >= '0' && str[i] <= '9') {
			result *= 10;
            result += (str[i] - '0');
        } else {
            return UINT32_MAX;
        }
    }

	return result;
}

char* strcpy(char* dest, const char* src)
{
	if(!dest || !src) return nullptr; 

	size_t src_len = strlen(src);
	return memcpy(dest, src, strlen(src) + 1); //add one for null term
}

char *strncpy(char *dest, const char *src, size_t max)
{
	const size_t len_src = strnlen(src, max);
	if(len_src < max){
		memcpy(dest, src, len_src);


		//memset(dest + len_src, 0, max - len_src);
		//okay some impls of libc seem to memset rest of dst to 0, but im just gonna do the first char after 
		//bc i foresee myself accidentally memseting unintended areas when destlen isnt sufficient

		dest[len_src] = 0;

		return dest; 
	} else{
		return memcpy(dest, src, len_src);
	}
   
}

char* strcat(char* dest, const char* src)
{
	if(!dest || !src) return nullptr; 


	size_t dest_len = strlen(dest);
	strcpy(dest + dest_len, src);
	return dest; 
}


char* strncat(char* dest, const char* src, size_t n)
{
   
    if (!dest || !src) return 0;
	/*
		char* dest_end = dest;
		while (*dest_end) {
			++dest_end;
		}

		size_t i = 0;
		while (src[i] && i < n) {
			dest_end[i] = src[i];
			++i;
		}
		dest_end[i] = '\0'; // Null-terminate the concatenated string */

    size_t dest_len = strlen(dest);
    strncpy(dest + dest_len, src, n);
    dest[dest_len + n] = '\0'; // Ensure null termination

    return dest;
}


size_t strlen(const char* str){ //could be more robust but we dont have malloc so we aint gettin any fancy strings anyway
	size_t len =0;
	while(str[len] != 0) len++;
	return len;
}

size_t strnlen(const char *str, size_t max)
{
	size_t len = 0;

	for (len = 0; len < max; ++len){
		if(str[len] == 0) break;
	}
	return len;
	
}	

int strncmp(const char *str1, const char* str2, size_t num)
{
	if(!str1 || !str2 || !num) return 0;	
	int matching = 0;
    int c = 0;
	while(str1[c] == str2[c] && c < num && str1[c] != 0 && str2[c] != 0){
		matching++;
		c++;
	}

	/*
		spec is < 0 when lower first not match in 1 v 2

		0 when equal 

		> 0 when greater first not match in 1 v 2
	*/ //this is gonna be wrong bc i wrote it to return num matching chars earlier 
	if(matching == num) return 0; //this is broken
	if(str1[c] > str2[c]) return -1; 
	else return 1; 

}

int strcmp(const char *str1, const char *str2)
{
	//it is too late to be writing stdlib funcs rn

	//sorry
	
	while (*str1 && *str2 && *str1 == *str2) {
        str1++;
        str2++;
    }

   
    if (*str1 == *str2) 
        return 0;  // Both strings are equal

	return  (*str1 < *str2) ? -1 : 1;
}
