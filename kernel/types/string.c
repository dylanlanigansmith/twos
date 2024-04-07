#include "string.h"

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
	//append 0x //TODO
    return itoa(val, 16);
}