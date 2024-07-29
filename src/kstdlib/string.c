#include <kstdlib/string.h>

size_t strlen(const char *str)
{
    size_t size = 0;
    while(str[size] != '\0') size++;

    return size; 
}
size_t strnlen(const char *str, size_t max)
{
    size_t len = 0;
    for (len = 0; len < max; ++len){
      if(str[len] == 0) break;
    }

    return len; 
}


char* strcpy(char* dest, const char* src)
{
	if(!dest || !src) return 0; 

	size_t src_len = strlen(src);
	return memcpy((void*)dest, (void*)src, strlen(src) + 1); //add one for null term
}
char *strncpy(char *dest, const char *src, size_t max) //THIS FUNCTION IMPL IS OLD AND BAD FROM OLD PROJECT 
{
	const size_t len_src = strnlen(src, max);
	if(len_src < max){
		memcpy((void*)dest, (void*)src, len_src);

		
		memset(dest + len_src, 0, max - len_src); //lets try it
		//okay some impls of libc seem to memset rest of dst to 0, but im just gonna do the first char after 
		//bc i foresee myself accidentally memseting unintended areas when destlen isnt sufficient

		dest[len_src] = 0;

		return dest; 
	} else{
		return memcpy((void*)dest, (void*)src, len_src); //i think we are relying on undefined behaviour here and its in our favour
	}
   
}

char* strcat(char* dest, const char* src)
{
	if(!dest || !src) return 0; 


	size_t dest_len = strlen(dest);
	strcpy(dest + dest_len, src);
	return dest; 
}
char * strncat(char *s1, const char *s2, size_t n) //remember this adds N chars, not UP TO N chars
{
    unsigned len1 = strlen(s1);
    unsigned len2 = strlen(s2);
    
    if (len2 < n) {
	strcpy(&s1[len1], s2);
    } else {
	strncpy(&s1[len1], s2, n);
	s1[len1 + n] = '\0';
    }
    return s1;
}




//stolen from https://stackoverflow.com/a/29933810 because i dont trust my prev. impl 
char *itoa(int a, char *dest, int base)
{
  char buffer[sizeof a * CHAR_BIT + 1 + 1]; 
  static const char digits[36] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  if (base < 2 || base > 36) {
    return 0;
  }

  // Start filling from the end
  char* p = &buffer[sizeof buffer - 1];
  *p = '\0';

  // Work with negative `int`
  int an = a < 0 ? a : -a;  

  do {
    *(--p) = digits[-(an % base)];
    an /= base;
  } while (an);

  if (a < 0) {
    *(--p) = '-';
  }

  size_t size_used = &buffer[sizeof(buffer)] - p;
 // if (size_used > size) {
  return memcpy(dest, p, size_used);
}
const char* zerostr = "0";
char *ltoa(uint64_t a, char *dest, int base)
{
  char buf[62] = {0};
	buf[31] = 0;
	//weird bug
	if(a == 0ull){
		return strcpy(dest, zerostr);
	}

	int i = 60;
	
	for(; a && i ; --i, a /= base)
		buf[i] = "0123456789abcdef"[a % base];
	

	return strncpy(dest, buf + i + 1, 32);
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