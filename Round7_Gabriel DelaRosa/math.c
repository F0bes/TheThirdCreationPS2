#include "math.h"

float math_sinetable[MATH_N][4];

void math_init()
{
	int i;
	float k=(2*pi/MATH_N);
	
	for(i=0;i<MATH_N;i++)
	{
		float a0=i*(2*pi/MATH_N);
		float a1=(i+1)*(2*pi/MATH_N);
		float v0,d0,v1,d1;
		
		v0=sin(a0);
		d0=cos(a0);
		v1=sin(a1);
		d1=cos(a1);

		math_sinetable[i][0]=v0;
		math_sinetable[i][1]=d0;
		math_sinetable[i][2]=(3*(v1-v0)-k*(d1+2*d0))/(k*k);
		math_sinetable[i][3]=(k*(d1+d0)-2*(v1-v0))/(k*k*k);
	}
}

