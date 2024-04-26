/**
 * 
 * 
 * so im just copying these from kernel stdlib which sucks for obvious reasons
*/
#include "string.h"
#include "stdlib.h"
const char* zero = "0";
char* _itoa(int val, int base){
	
	static char buf[32] = {0};
	
	//weird bug
	if(val == 0){
		return zero;
	}

	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}
char* _lltoa(uint64_t val, int base){
	
	static char buf[62] = {0};
	
	//weird bug
	if(val == 0u){
		return zero;
	}

	int i = 60;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}

char*  itoa(int val, char *buffer, unsigned int base)
{
	char buf[32] = {0};
	buf[31] = 0;
	//weird bug
	if(val == 0){
		return strcpy(buffer, zero);
	}

	int i = 30;
	
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	
	return strcpy(buffer, &buf[i+1]) ;
}

char* lltoa(uint64_t val, char *buffer, unsigned int base) //give it 32 chars at least
{
	char buf[32] = {0};
	buf[31] = 0;
	//weird bug
	if(val == 0ull){
		return strcpy(buffer, zero);
	}

	int i = 30;
	
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	

	return strcpy(buffer, &buf[i+1]) ;
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

int atoi(const char *str) //returns max uint32 on failure
{
    int result = 0;
   
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



char* strcpy(char* dest, const char* src)
{
	if(!dest || !src) return nullptr; 

	size_t src_len = strlen(src);
	return memcpy((void*)dest, (void*)src, strlen(src) + 1); //add one for null term
}

char *strncpy(char *dest, const char *src, size_t max)
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
char* strncpy2(register char* t, register const char* f, size_t n) //uh
{
	register char*	e = t + n - 1;

	do
	{
		if (t >= e)
		{
			*t = 0;
			return t;
		}
	} while (*t++ = *f++);
	return t - 1;
}



char* strcat(char* dest, const char* src)
{
	if(!dest || !src) return nullptr; 


	size_t dest_len = strlen(dest);
	strcpy(dest + dest_len, src);
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
	/*if(!str1 || !str2 || !num) return 0;	
	int matching = 0;
    int c = 0;
	while(str1[c] == str2[c] && c < num && str1[c] != 0 && str2[c] != 0){
		matching++;
		c++;
	}

	
	if(matching == num) return 0; 
	if(str1[c] > str2[c]) return -1; 
	else return 1; 
*/
 if (!str1 || !str2 || !num) {
        return 0;
    }

    size_t c = 0;
    while (c < num && str1[c] != 0 && str2[c] != 0) {
        if (str1[c] != str2[c]) {
            unsigned char uc1 = (unsigned char)str1[c];
            unsigned char uc2 = (unsigned char)str2[c];
            if (uc1 > uc2) {
                return 1;
            } else {
                return -1;
            }
        }
        c++;
    }

    if (c == num) {
        return 0;
    }

    if (str1[c] == 0) {
        return -1;
    } else if (str2[c] == 0) {
        return 1;
    }

    unsigned char uc1 = (unsigned char)str1[c];
    unsigned char uc2 = (unsigned char)str2[c];
    if (uc1 > uc2) {
        return 1;
    } else {
        return -1;
    }
}

int strcmp(const char *str1, const char *str2)
{
	size_t i = 0;
	while(str1[i] == str2[i]){
		if(str1[i++] == 0) return 0;
	}

    return (str1[i] > str2[i]) ? 1 : -1;
}

int strcasecmp(const char *s1, const char *s2)
{
    const unsigned char *us1 = (const unsigned char *)s1;
    const unsigned char *us2 = (const unsigned char *)s2;

    while (tolower(*us1) == tolower(*us2)) {
        if (*us1 == '\0' || *us2 == '\0')
            break;
        us1++;
        us2++;
    }
    return tolower(*us1) - tolower(*us2);
}

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    if (n != 0) {
        const unsigned char *us1 = (const unsigned char *)s1;
        const unsigned char *us2 = (const unsigned char *)s2;

        do {
            if (tolower(*us1) != tolower(*us2))
                return tolower(*us1) - tolower(*us2);
            if (*us1 == '\0' || *us2 == '\0')
                break;
            us1++;
            us2++;
        } while (--n != 0);
    }
    return 0;
}


char *strdup(const char *s){
	size_t l = strlen(s);

	char* str = malloc(l + 1);
	str[l] = 0;
	
	return strcpy(str, s);
}

char * strstr (const char * str1, const char * str2 ){
	return 0;
 }
int toupper(char c)
 {
	if('a' <= c && c <= 'z')
     	return c - 0x20;
	return c;
 }
int tolower(char c){
    if('A' <= c && c <= 'Z'){
        return c + 0x20 ; 
    }
    return c;
}


/*
int strcasecmp(const char *s1, const char *s2)
{

	const unsigned char
			*us1 = (const unsigned char *)s1,
			*us2 = (const unsigned char *)s2;

	while (tolower(*us1) == tolower(*us2++))
		if (*us1++ == '\0')
			return (0);
	return (tolower(*us1) - tolower(*--us2));

}

int strncasecmp(const char* s1, const char* s2, size_t n)
{
	if (n != 0) {
			const unsigned char
					*us1 = (const unsigned char *)s1,
					*us2 = (const unsigned char *)s2;

			do {
				if (tolower(*us1) != tolower(*us2++))
					return (tolower(*us1) - tolower(*--us2));
				if (*us1++ == '\0')
					break;
			} while (--n != 0);
		}
		return (0);
}*/