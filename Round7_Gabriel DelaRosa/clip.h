//---------------------------------------------------------------------------
// File:	clip.h
// Author:	Gabriel T. DelaRosa
//
#ifndef _CLIP_H
#define _CLIP_H
#include "types.h"


typedef struct
{
	float x;
	float y;
	float z;
	float s;
	float t;
} vertx  __attribute__((aligned(16)));

float calc_new_coord(float x1,float y1,float x2,float y2,
						float ylim);

int clip_above(fvec *clipped_vpts,fvec *vpts, float ymin);

int clip_below(fvec *clipped_vpts,fvec *vpts, float ymax);

int clip_near(vertx *clipped_pts,vertx *pts, float zmin);




#endif