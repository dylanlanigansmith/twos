#include "string.h"
#include "memory.h"

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

char* htoa(int val)
{
	//untested and bad
	const char prefix[2] = {"0x"};
	static char buf[34] = {0};
	buf[0] = prefix[0];
	buf[1] = prefix[1];
	buf[2] = 0;
	return strcat(buf, itoa(val, 16));
}

char* strcpy(char* dest, const char* src)
{
	return memcpy(dest, src, strlen(src) + 1); //add one for null term
}

char* strcat(char* dest, const char* src)
{
	strcpy(dest + strlen(dest), src);
	return dest; 
}


size_t strlen(const char* str){ //could be more robust but we dont have malloc so we aint gettin any fancy strings anyway
	size_t len =0;
	while(str[len] != 0) len++;
	return len;
}