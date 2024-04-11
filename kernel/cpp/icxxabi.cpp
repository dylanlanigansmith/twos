#include "icxxabi.hpp"
extern "C"
{



struct atexit_func_entry_t
{
	/*
	* Each member is at least 4 bytes large. Such that each entry is 12bytes.
	* 128 * 12 = 1.5KB exact.
	**/
	void (*destructor_func)(void *);
	void *obj_ptr;
	void *dso_handle;
};

uarch_t __atexit_func_count = 0;
void *__dso_handle = 0;
atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];

int __cxa_atexit(void (*f)(void *), void *objptr, void *dso)
{ 
    if (__atexit_func_count >= ATEXIT_MAX_FUNCS) {return -1;};
	__atexit_funcs[__atexit_func_count].destructor_func = f;
	__atexit_funcs[__atexit_func_count].obj_ptr = objptr;
	__atexit_funcs[__atexit_func_count].dso_handle = dso;
	__atexit_func_count++;
	return 0;
}
void __cxa_finalize(void *f){ //dls: this is hella broken but it aint ever gonna get called
   uarch_t i = __atexit_func_count;
	if (!f)
	{

		while (i--)
		{
			if (__atexit_funcs[i].destructor_func)
			{
				(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
			};
		};
		return;
	};
 
	while (i--)
	{
		if (__atexit_funcs[i].destructor_func == f)
		{
			
			(*__atexit_funcs[i].destructor_func)(__atexit_funcs[i].obj_ptr);
			__atexit_funcs[i].destructor_func = 0;
 
		};
	};
}

void __cxa_pure_virtual()
{
    __asm__ volatile ("nop");
}

}