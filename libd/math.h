#pragma once

#include "stdint.h"

static inline int abs(int x){
    return (x > 0) ? x : x * -1;
}


static inline double fabs(double x)
{
	union
	{
		double f;
		uint64_t i;
	} u = {x};
	u.i &= -1ULL / 2;
	return u.f;
}