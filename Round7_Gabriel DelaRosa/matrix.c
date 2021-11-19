#include "math.h"
#include "matrix.h"

#define sin sin2
#define cos cos2

void	resetmatrixf(fmatrix *m)
{
	m->xu=m->yv=m->zw=1;
	m->xv=m->xw=m->xx=m->yu=m->yw=m->yy=m->zu=m->zv=m->zz=0;
}

void	scalematrixf(fmatrix *m, float x, float y, float z)
{
	m->xu*=x;
	m->xv*=x;
	m->xw*=x;
	m->xx*=x;
	m->yu*=y;
	m->yv*=y;
	m->yw*=y;
	m->yy*=y;
	m->zu*=z;
	m->zv*=z;
	m->zw*=z;
	m->zz*=z;
}

void	rotatematrixzf(fmatrix *m, float v)
{
	float	s,c;
	float	t;

        s=sin(v);
        c=cos(v);

	t=m->xu;
	m->xu=t*c-m->yu*s;
	m->yu=t*s+m->yu*c;

	t=m->xv;
	m->xv=t*c-m->yv*s;
	m->yv=t*s+m->yv*c;

	t=m->xw;
	m->xw=t*c-m->yw*s;
	m->yw=t*s+m->yw*c;

	t=m->xx;
	m->xx=t*c-m->yy*s;
	m->yy=t*s+m->yy*c;
}

void	rotatematrixyf(fmatrix *m, float v)
{
	float	s,c;
	float	t;

        s=sin(v);
        c=cos(v);

	t=m->zu;
	m->zu=t*c-m->xu*s;
	m->xu=t*s+m->xu*c;

	t=m->zv;
	m->zv=t*c-m->xv*s;
	m->xv=t*s+m->xv*c;

	t=m->zw;
	m->zw=t*c-m->xw*s;
	m->xw=t*s+m->xw*c;

	t=m->zz;
	m->zz=t*c-m->xx*s;
	m->xx=t*s+m->xx*c;
}

void	rotatematrixxf(fmatrix *m, float v)
{
	float	s,c;
	float	t;

        s=sin(v);
        c=cos(v);

	t=m->yu;
	m->yu=t*c-m->zu*s;
	m->zu=t*s+m->zu*c;

	t=m->yv;
	m->yv=t*c-m->zv*s;
	m->zv=t*s+m->zv*c;

	t=m->yw;
	m->yw=t*c-m->zw*s;
	m->zw=t*s+m->zw*c;

	t=m->yy;
	m->yy=t*c-m->zz*s;
	m->zz=t*s+m->zz*c;
}

void	movematrixf(fmatrix *m, float x, float y, float z)
{
	m->xx+=x;
	m->yy+=y;
	m->zz+=z;
}

void	fmovematrixf(fmatrix *m, float x, float y, float z)
{
	m->xx+=(x*(m->xu))+(y*(m->xv))+(z*(m->xw));
	m->yy+=x*m->yu+y*m->yv+z*m->yw;
	m->zz+=x*m->zu+y*m->zv+z*m->zw;
}

void	frotatematrixzf(fmatrix *m, float v)
{
	float	s,c;
	float	t;

        s=sin(v);
        c=cos(v);

	t=m->xu;
	m->xu=t*c+m->xv*s;
	m->xv=-t*s+m->xv*c;

	t=m->yu;
	m->yu=t*c+m->yv*s;
	m->yv=-t*s+m->yv*c;

	t=m->zu;
	m->zu=t*c+m->zv*s;
	m->zv=-t*s+m->zv*c;
}

void	frotatematrixyf(fmatrix *m, float v)
{
	float	s,c;
	float	t;

        s=sin(v);
        c=cos(v);

	t=m->xw;
	m->xw=t*c+m->xu*s;
	m->xu=-t*s+m->xu*c;

	t=m->yw;
	m->yw=t*c+m->yu*s;
	m->yu=-t*s+m->yu*c;

	t=m->zw;
	m->zw=t*c+m->zu*s;
	m->zu=-t*s+m->zu*c;
}

void	frotatematrixxf(fmatrix *m, float v)
{
	float	s,c;
	float	t;

        s=sin(v);
        c=cos(v);

	t=m->xv;
	m->xv=t*c+m->xw*s;
	m->xw=-t*s+m->xw*c;

	t=m->yv;
	m->yv=t*c+m->yw*s;
	m->yw=-t*s+m->yw*c;

	t=m->zv;
	m->zv=t*c+m->zw*s;
	m->zw=-t*s+m->zw*c;
}

void    fscalematrixf(fmatrix *m, float x, float y, float z)
{
	m->xu*=x;
	m->yu*=x;
	m->zu*=x;
	m->xv*=y;
	m->yv*=y;
	m->zv*=y;
	m->xw*=z;
	m->yw*=z;
	m->zw*=z;
}

void    fmulmatrixf(fmatrix *m, fmatrix *n)
{
        //m=m*n

        fmatrix temp;

        temp=*m;

        m->xu=temp.xu*n->xu+temp.xv*n->yu+temp.xw*n->zu;
        m->xv=temp.xu*n->xv+temp.xv*n->yv+temp.xw*n->zv;
        m->xw=temp.xu*n->xw+temp.xv*n->yw+temp.xw*n->zw;
        m->xx=temp.xu*n->xx+temp.xv*n->yy+temp.xw*n->zz+temp.xx;

        m->yu=temp.yu*n->xu+temp.yv*n->yu+temp.yw*n->zu;
        m->yv=temp.yu*n->xv+temp.yv*n->yv+temp.yw*n->zv;
        m->yw=temp.yu*n->xw+temp.yv*n->yw+temp.yw*n->zw;
        m->yy=temp.yu*n->xx+temp.yv*n->yy+temp.yw*n->zz+temp.yy;

        m->zu=temp.zu*n->xu+temp.zv*n->yu+temp.zw*n->zu;
        m->zv=temp.zu*n->xv+temp.zv*n->yv+temp.zw*n->zv;
        m->zw=temp.zu*n->xw+temp.zv*n->yw+temp.zw*n->zw;
        m->zz=temp.zu*n->xx+temp.zv*n->yy+temp.zw*n->zz+temp.zz;
}

void    mulmatrixf(fmatrix *m, fmatrix *n)
{
        //m=n*m

        fmatrix temp;

        temp=*m;

        m->xu=n->xu*temp.xu+n->xv*temp.yu+n->xw*temp.zu;
        m->xv=n->xu*temp.xv+n->xv*temp.yv+n->xw*temp.zv;
        m->xw=n->xu*temp.xw+n->xv*temp.yw+n->xw*temp.zw;
        m->xx=n->xu*temp.xx+n->xv*temp.yy+n->xw*temp.zz+n->xx;

        m->yu=n->yu*temp.xu+n->yv*temp.yu+n->yw*temp.zu;
        m->yv=n->yu*temp.xv+n->yv*temp.yv+n->yw*temp.zv;
        m->yw=n->yu*temp.xw+n->yv*temp.yw+n->yw*temp.zw;
        m->yy=n->yu*temp.xx+n->yv*temp.yy+n->yw*temp.zz+n->yy;

        m->zu=n->zu*temp.xu+n->zv*temp.yu+n->zw*temp.zu;
        m->zv=n->zu*temp.xv+n->zv*temp.yv+n->zw*temp.zv;
        m->zw=n->zu*temp.xw+n->zv*temp.yw+n->zw*temp.zw;
        m->zz=n->zu*temp.xx+n->zv*temp.yy+n->zw*temp.zz+n->zz;
}


void	rotatematrixf(fmatrix *m, float x, float y, float z, float v)
{
	float	i,j;

        i=-atan2(y,x);
        j=atan2(z,sqrt(x*x+y*y));

	rotatematrixzf(m,i);
	rotatematrixyf(m,j);
	rotatematrixxf(m,v);
	rotatematrixyf(m,-j);
	rotatematrixzf(m,-i);
}


void    transformf(fmatrix *m, fvec *s, fvec *d, int n)
{
	int	t;

        for(t=0;t<n;t++)
	{
		d[t].x=m->xu*s[t].x+m->xv*s[t].y+m->xw*s[t].z+m->xx;
		d[t].y=m->yu*s[t].x+m->yv*s[t].y+m->yw*s[t].z+m->yy;
		d[t].z=m->zu*s[t].x+m->zv*s[t].y+m->zw*s[t].z+m->zz;
	}
}

void    transformf2(fmatrix *m, fvec *s, fvec *d, int n)
{
	int	t;

  __asm__ __volatile__("
		lqc2	vf8, 0x0(%2)
		lqc2	vf9, 0x10(%2)
		lqc2	vf10, 0x20(%2)
		lqc2	vf11, 0x30(%2)

		"
		 :: "r" (&(s[t])),"r" (&(d[t])),"r" (m));

        for(t=0;t<n;t++)
	{
  __asm__ __volatile__("
		lqc2	vf4, 0x0(%0)

		vmulax.xyz	ACC,vf8,vf4x
		VMADDAy.xyz	ACC,vf9,vf4y
		vmaddz.xyz	vf5,vf10,vf4z
		vadd	vf5,vf11,vf5

		sqc2	vf5,0x0(%1)
		"
		 :: "r" (&(s[t])),"r" (&(d[t])),"r" (m));
	}
}


// Added by	Gabriel T. DelaRosa
/////////////////////////////////////////////////////////////////////////////////
void	identitymatrixf(fmatrix *m) 
{
	m->xu = m->yv = m->zw = m->dummy4=1;

	m->xv = m->xw = m->xx = m->yu = m->yw = m->yy = 
	m->zu = m->zv = m->zz = m->dummy1 = m->dummy2 = m->dummy3 = 0;
}


void setviewmatrixf(fmatrix *m,fmatrix *n)// keep
{
	n->xu=m->xu; n->xv=m->yu; n->xw=m->zu; n->xx= -(m->xx*m->xu + m->yy*m->yu + m->zz*m->zu);
	n->yu=m->xv; n->yv=m->yv; n->yw=m->zv; n->yy= -(m->xx*m->xv + m->yy*m->yv + m->zz*m->zv);
	n->zu=m->xw; n->zv=m->yw; n->zw=m->zw; n->zz= -(m->xx*m->xw + m->yy*m->yw + m->zz*m->zw);
	n->dummy1=0; n->dummy2=0; n->dummy3=0; n->dummy4=1.0f;
}

void    transformf3(fmatrix *m, fvec *s, fvec *d, int n)
{
	int	t;

    for(t=0;t<n;t++)
	{
		d[t].x=m->xu*s[t].x+m->xv*s[t].y+m->xw*s[t].z+m->xx*s[t].w;
		d[t].y=m->yu*s[t].x+m->yv*s[t].y+m->yw*s[t].z+m->yy*s[t].w;
		d[t].z=m->zu*s[t].x+m->zv*s[t].y+m->zw*s[t].z+m->zz*s[t].w;
		d[t].w=m->dummy1*s[t].x+m->dummy2*s[t].y+m->dummy3*s[t].z+m->dummy4*s[t].w;
	}
}
//////////////////////////////////////////////////////////////////////////////

