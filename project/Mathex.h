#pragma once
#pragma once
#ifndef mathex_h

#define mathex_h



#include <math.h>



#define EPSILON 1.0e-8

#define ZERO EPSILON

#define M_PI 3.1415926535



template <class T> inline T sqr(const T &x)

{

	return x * x;

}


#endif

