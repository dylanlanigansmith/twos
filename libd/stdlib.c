#include "stdlib.h"
//THE PAIN OF MEMCPY AND MEMSET
//using register due to trust issues and trauma
//itrps firing during memset cost me like 10 hrs back in kernel mode
//i have spent a lot of time in IDA because of these issues
//memcpy fails on physical hardware...
//but only when doom uses it, i cant recreate it with test data and allocations in the same place
//and everything works in every VM...
//AHGHGHGHGH
//something to do with possibly itrps, paging, and maybe tlb who fuckin knows
//could impl a kernel memcpy syscall for shits to see if that helps
//but debugging this on the same machine i am developing with sucks, 
//^^ esp when bad crashes dont even throw exceptions and lock up mobo until u full power cycle

void* memcpy(void* dest, void* src, register size_t num_bytes){
    if(!src ||!dest || !num_bytes) return 0;
    register uint8_t* d = dest;
    register const uint8_t* s = src;
    while (num_bytes--)
        *d++ = *s++;
    return dest;
}

void *memcpy_fastnfurious(void *dest, void *src, size_t num_bytes) 
{
	//fast bc 64 bits at a time. furious bc we dont support anything but 64 bit aligned sizes.
	 if(!src ||!dest || !num_bytes) return 0;
    if(num_bytes % sizeof(uint64_t) > 0) return 0;

	
	register uint64_t* d = dest;
    register const uint64_t* s = src;

	while (num_bytes -= 8)
        *d++ = *s++;


    return dest;
}

/*
this memcpy2 is from apple libc src and was used for debugging weird behaviour on physical hardware
*/


typedef	int word;		/* "word" used for optimal copy speed */

#define	wsize	sizeof(word)
#define	wmask	(wsize - 1)

void *memcpy2(void *dst0, const void *src0, size_t length)
{
	char *dst = dst0;
	const char *src = src0;
	size_t t;
	
	if (length == 0 || dst == src)		/* nothing to do */
		goto done;
	
	/*
	 * Macros: loop-t-times; and loop-t-times, t>0
	 */
#define	TLOOP(s) if (t) TLOOP1(s)
#define	TLOOP1(s) do { s; } while (--t)
	
	if ((unsigned long)dst < (unsigned long)src) {
		/*
		 * Copy forward.
		 */
		t = (uintptr_t)src;	/* only need low bits */
		if ((t | (uintptr_t)dst) & wmask) {
			/*
			 * Try to align operands.  This cannot be done
			 * unless the low bits match.
			 */
			if ((t ^ (uintptr_t)dst) & wmask || length < wsize)
				t = length;
			else
				t = wsize - (t & wmask);
			length -= t;
			TLOOP1(*dst++ = *src++);
		}
		/*
		 * Copy whole words, then mop up any trailing bytes.
		 */
		t = length / wsize;
		TLOOP(*(word *)dst = *(word *)src; src += wsize; dst += wsize);
		t = length & wmask;
		TLOOP(*dst++ = *src++);
	} else {
		/*
		 * Copy backwards.  Otherwise essentially the same.
		 * Alignment works as before, except that it takes
		 * (t&wmask) bytes to align, not wsize-(t&wmask).
		 */
		src += length;
		dst += length;
		t = (uintptr_t)src;
		if ((t | (uintptr_t)dst) & wmask) {
			if ((t ^ (uintptr_t)dst) & wmask || length <= wsize)
				t = length;
			else
				t &= wmask;
			length -= t;
			TLOOP1(*--dst = *--src);
		}
		t = length / wsize;
		TLOOP(src -= wsize; dst -= wsize; *(word *)dst = *(word *)src);
		t = length & wmask;
		TLOOP(*--dst = *--src);
	}
done:
	return (dst0);
}



void* memset(void* dest, register uint8_t val, register size_t num_bytes){
    register uint8_t *d = (uint8_t*)dest;
    while(num_bytes-- > 0)
        *d++ = val;
    return dest;
}