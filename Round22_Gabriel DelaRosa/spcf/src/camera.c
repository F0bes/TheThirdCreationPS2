/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/
#include "camera.h"
#include "math.h"

//calc rotation matrix based on euler angles------------------------
void rotatecamera(mcamera *m, float xaxis, float yaxis, float zaxis)
{
	float cr,sr,cp,sp,cy,sy,srsp,crsp;

	if (!m)
		return;

	identitymatrixf(m);

	cr = cos(xaxis*.01745f);
	sr = sin(xaxis*.01745f);

	cp = cos(yaxis*.01745f);
	sp = sin(yaxis*.01745f);

	cy = cos(zaxis*.01745f);
	sy = sin(zaxis*.01745f);


	m->xu = (cp*cy);
	m->yu = (cp*sy);
	m->zu = (-sp);

	srsp = sr*sp;
	crsp = cr*sp;

	m->xv = (srsp*cy-cr*sy);
	m->yv = (srsp*sy+cr*cy);
	m->zv = (sr*cp);

	m->xw = (crsp*cy+sr*sy);
	m->yw = (crsp*sy-sr*cy);
	m->zw = (cr*cp);
}


//-----------------------------------------------------------------------------------------
void init_camera(mcamera *camera)
{
	if (!camera)
		return;
	identitymatrixf(camera);
}

//---------------------------------------------------------------------------------------------
void position_camera(mcamera *camera, float x, float y, float z, float pt, float yw, float rl)
{
	if (!camera)
		return;
	rotatecamera(camera, pt, yw, rl);
	movematrixf(camera, x, y, z);
}


//------------------------------------------------------------------------------------------
void set_cam_lookat(mcamera *camera, fvec eye, fvec lookat)
{
	fvec pt[3];

	if (!camera)
		return;

	identitymatrixf(camera);

	camera->xx = eye.x;
	camera->yy = eye.y;
	camera->zz = eye.z;

	pt[0].x = (camera->xx-lookat.x);
	pt[0].y = (camera->yy-lookat.y);
	pt[0].z = (camera->zz-lookat.z);
	norm(&pt[0]);
	camera->xw = pt[0].x;
	camera->yw = pt[0].y;
	camera->zw = pt[0].z;

	pt[1].x = 0;
	pt[1].y = 1;
	pt[1].z = 0;
	crossprod(pt[0],pt[1],&pt[2]);
	norm(&pt[2]);
	camera->xu = pt[2].x;
	camera->yu = pt[2].y;
	camera->zu = pt[2].z;

	crossprod(pt[0],pt[2],&pt[1]);
	norm(&pt[1]);
	camera->xv = pt[1].x;
	camera->yv = pt[1].y;
	camera->zv = pt[1].z;
}
