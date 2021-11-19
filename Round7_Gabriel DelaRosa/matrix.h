#ifndef matrix34
#define matrix34

#include "types.h"

typedef struct fmatrix{
/*	float	xu,xv,xw,xx;
	float	yu,yv,yw,yy;
	float	zu,zv,zw,zz;
*/
	float	xu,yu,zu,dummy1;
	float	xv,yv,zv,dummy2;
	float	xw,yw,zw,dummy3;
	float	xx,yy,zz,dummy4;
} fmatrix __attribute__((aligned(16)));

extern void	resetmatrixf(fmatrix *m);
extern void	scalematrixf(fmatrix *m, float x, float y, float z);
extern void	rotatematrixzf(fmatrix *m, float v);
extern void	rotatematrixyf(fmatrix *m, float v);
extern void	rotatematrixxf(fmatrix *m, float v);
extern void	movematrixf(fmatrix *m, float x, float y, float z);
extern void	fmovematrixf(fmatrix *m, float x, float y, float z);
extern void	frotatematrixzf(fmatrix *m, float v);
extern void	frotatematrixyf(fmatrix *m, float v);
extern void	frotatematrixxf(fmatrix *m, float v);
extern void    fscalematrixf(fmatrix *m, float x, float y, float z);
extern void    fmulmatrixf(fmatrix *m, fmatrix *n);
extern void    mulmatrixf(fmatrix *m, fmatrix *n);
extern void	rotatematrixf(fmatrix *m, float x, float y, float z, float v);
extern void    transformf(fmatrix *m, fvec *s, fvec *d, int n);
extern void    transformf2(fmatrix *m, fvec *s, fvec *d, int n);

// Added by	Gabriel T. DelaRosa
///////////////////////////////////////////////////////////////////////////////////
extern void	identitymatrixf(fmatrix *m);
extern void	setmatrixf(fmatrix *m, float *rot);
extern void	setviewmatrixf(fmatrix *m,fmatrix *n);
extern void transformf3(fmatrix *m, fvec *s, fvec *d, int n);

#endif