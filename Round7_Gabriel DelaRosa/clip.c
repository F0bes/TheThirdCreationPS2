//---------------------------------------------------------------------------
// File:	clip.c
// Author:	Gabriel T. DelaRosa
//
#include "clip.h"

float calc_new_coord(float x1,float y1,float x2,float y2,float ylim)
{
	float m;

	if (x1 != x2)		
	{					
		m= (y2-y1) / (x2-x1);					
		return (x1 + ((ylim - y1) / m));
	}else			
		return x1;

	return 0;
}

int clip_above(fvec *clipped_vpts,fvec *vpts, float ymin)
{
	int v1,v2;
	float x1,y1,z1,x2,y2,z2,newx,newz;
	int cp = 0;
	v1=3;
	for (v2=0; v2 < 4; v2++)
	{
		x1 = vpts[v1].x; 
		y1 = vpts[v1].y; 
		z1 = vpts[v1].z; 

		x2 = vpts[v2].x; 
		y2 = vpts[v2].y; 
		z2 = vpts[v2].z; 

		if ((y1 >= ymin) && (y2 >= ymin))
		{
			clipped_vpts[cp  ].x = x2;
			clipped_vpts[cp  ].y = y2;
			clipped_vpts[cp++].z = z2;
		}
		else if ((y1 < ymin) && (y2 < ymin))
		{
			// completely below
		}
		if ((y1 >= ymin) && (y2 < ymin))
		{
			newx = calc_new_coord(x1,y1,x2,y2,ymin);
			newz = calc_new_coord(z1,y1,z2,y2,ymin);

			clipped_vpts[cp  ].x = newx;
			clipped_vpts[cp  ].y = ymin;
			clipped_vpts[cp++].z = newz;
		}
		if ((y1 < ymin) && (y2 >= ymin)) 
		{
			newx = calc_new_coord(x1,y1,x2,y2,ymin);
			newz = calc_new_coord(z1,y1,z2,y2,ymin);

			clipped_vpts[cp  ].x = newx;
			clipped_vpts[cp  ].y = ymin;
			clipped_vpts[cp++].z = newz;

			clipped_vpts[cp  ].x = x2;
			clipped_vpts[cp  ].y = y2;
			clipped_vpts[cp++].z = z2;
		}
		v1=v2;
	}

	return cp;
	
}


int clip_below(fvec *clipped_vpts,fvec *vpts, float ymax)
{
	int v1,v2;
	float x1,y1,z1,x2,y2,z2,newx,newz;
	int cp = 0;
	v1=3;
	for (v2=0; v2 < 4; v2++)
	{
		x1 = vpts[v1].x;
		y1 = vpts[v1].y;
		z1 = vpts[v1].z;

		x2 = vpts[v2].x;
		y2 = vpts[v2].y;
		z2 = vpts[v2].z;

		if ((y1 <= ymax) && (y2 <= ymax))
		{
			clipped_vpts[cp  ].x = x2;
			clipped_vpts[cp  ].y = y2;
			clipped_vpts[cp++].z = z2;
		}
		else if ((y1 > ymax) && (y2 > ymax))
		{
			// completely above
		}
		if ((y1 <= ymax) && (y2 > ymax))
		{
			newx = calc_new_coord(x1,y1,x2,y2,ymax);
			newz = calc_new_coord(z1,y1,z2,y2,ymax);
			clipped_vpts[cp  ].x = newx;
			clipped_vpts[cp  ].y = ymax;
			clipped_vpts[cp++].z = newz;
		}
		if ((y1 > ymax) && (y2 <= ymax)) 
		{
			newx = calc_new_coord(x1,y1,x2,y2,ymax);
			newz = calc_new_coord(z1,y1,z2,y2,ymax);
			clipped_vpts[cp  ].x = newx;
			clipped_vpts[cp  ].y = ymax;
			clipped_vpts[cp++].z = newz;

			clipped_vpts[cp  ].x = x2;
			clipped_vpts[cp  ].y = y2;
			clipped_vpts[cp++].z = z2;
		}
		v1=v2;
	}

	return cp;
	
}


int clip_near(vertx *clipped_pts,vertx *pts, float zmin)
{
	int v1,v2;
	float x1,y1,z1,x2,y2,z2,newy,newx,scale;
	int cp = 0;
	v1=3;
	for (v2=0; v2 < 4; v2++)
	{
		x1 = pts[v1].x; 
		y1 = pts[v1].y; 
		z1 = pts[v1].z; 

		x2 = pts[v2].x; 
		y2 = pts[v2].y; 
		z2 = pts[v2].z; 

		if ((z1 >= zmin) && (z2 >= zmin))
		{
			clipped_pts[cp  ].x = x2;
			clipped_pts[cp  ].y = y2;
			clipped_pts[cp  ].z = z2;
			clipped_pts[cp  ].s = pts[v2].s;
			clipped_pts[cp++].t = pts[v2].t;
		}
		else if ((z1 < zmin) && (z2 < zmin))
		{
			// both too close
		}
		if ((z1 >= zmin) && (z2 < zmin))
		{
			newx = calc_new_coord(x1,z1,x2,z2,zmin);
			newy = calc_new_coord(y1,z1,y2,z2,zmin);

			clipped_pts[cp  ].x = newx;
			clipped_pts[cp  ].y = newy;
			clipped_pts[cp  ].z = zmin;
			scale = (z1 - z2) / (z1 - zmin);
			clipped_pts[cp  ].s = pts[v2].s / scale;
			clipped_pts[cp++].t = pts[v2].t / scale;
		}
		if ((z1 < zmin) && (z2 >= zmin)) 
		{
			newx = calc_new_coord(x1,z1,x2,z2,zmin);
			newy = calc_new_coord(y1,z1,y2,z2,zmin);

			clipped_pts[cp  ].x = newx;
			clipped_pts[cp  ].y = newy;
			clipped_pts[cp  ].z = zmin;
			scale = (z2 - z1) / (z2 - zmin);
			clipped_pts[cp  ].s = pts[v1].s / scale;
			clipped_pts[cp++].t = pts[v1].t / scale;

			clipped_pts[cp  ].x = x2;
			clipped_pts[cp  ].y = y2;
			clipped_pts[cp  ].z = z2;
			clipped_pts[cp  ].s = pts[v2].s;
			clipped_pts[cp++].t = pts[v2].t;
		}
		v1=v2;
	}

	return cp;
	
}
