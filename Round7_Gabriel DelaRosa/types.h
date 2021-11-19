#ifndef __DATATYPES_
#define __DATATYPES_

#include "defines.h"
/*typedef char int8;
typedef short int16;
typedef int int32;
typedef long int64;
typedef struct int128
{
	int64 lo,hi;
}int128 __attribute__((aligned(16)));
*/
typedef struct fvec
{
	float x,y,z,w;
}fvec __attribute__((aligned(16)));

typedef struct ivec
{
	int32 x,y,z,w;
}ivec __attribute__((aligned(16)));

typedef union vec{
	int128 int128;
	fvec fvec;
	ivec ivec;
}vec __attribute__((aligned(16)));

static fvec new_fvec(float x, float y, float z)
{
	fvec v;
	
	v.x=x;
	v.y=y;
	v.z=z;
	v.w=0;

	return v;
}

#endif