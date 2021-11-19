#ifndef __my_math
#define __my_math
#define __my_math_iter 20
#define pi 3.14159265358
#define MATH_N (32)

extern float math_sinetable[MATH_N][4];

void math_init();

inline static float max(float a, float b){if(a>b)return a;return b;}
inline static float min(float a, float b){if(a<b)return a;return b;}

inline static float sqrt(float a)
{
	float b;
	
	asm(
	"sqrt.s %0,%1;"
	: "=f" (b)
	: "f" (a));
	
	return b;
}

// Polynomial approximation using sine table (degree 3)
inline static float sin2(float v)
{
	int i=((int)((v)*(1.0/(2*pi/MATH_N))+10000))-10000.0;
	v-=i*(2*pi/MATH_N);
	i&=MATH_N-1;

	return v*v*v*math_sinetable[i][3]+v*v*math_sinetable[i][2]+v*math_sinetable[i][1]+math_sinetable[i][0];
}

inline static float cos2(float v)
{
	int i=((int)((v)*(1.0/(2*pi/MATH_N))+10000))-10000.0;
	v-=i*(2*pi/MATH_N);
	i+=MATH_N/4;
	i&=MATH_N-1;

	return v*v*v*math_sinetable[i][3]+v*v*math_sinetable[i][2]+v*math_sinetable[i][1]+math_sinetable[i][0];
}

// Taylor series
static float cos(float v)
{
	float res,w;
	int t;
	float fac;
	int i=(v)/(2*pi);
	v-=i*2*pi;

	fac=1;
	res=0;
	w=1;
	for(t=0;t<__my_math_iter;)
	{
		res+=fac*w;
		w*=v*v;
		t++;
		fac/=t;
		t++;
		fac/=t;
		
		res-=fac*w;
		w*=v*v;
		t++;
		fac/=t;
		t++;
		fac/=t;
	}
	return res;
}

static float sin(float v)
{
	float res,w;
	int t;
	float fac;
	int i=(v)/(2*pi);
	v-=i*2*pi;

	fac=1;
	res=0;
	w=v;
	for(t=1;t<__my_math_iter;)
	{
		res+=fac*w;
		w*=v*v;
		t++;
		fac/=t;
		t++;
		fac/=t;
		
		res-=fac*w;
		w*=v*v;
		t++;
		fac/=t;
		t++;
		fac/=t;
	}
	return res;
}

static float atan2(float y, float x)
{
	// ikke implementeret
	return y/x;
}

#endif