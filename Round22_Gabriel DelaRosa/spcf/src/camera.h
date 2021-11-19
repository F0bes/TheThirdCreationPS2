/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/

#ifndef _CAMERA_H
#define _CAMERA_H

#ifndef __DATATYPES_
#include "types.h"
#endif
#ifndef __matrix34_
#include "matrix.h"
#endif

#define mcamera fmatrix // camera type is just a matrix
mcamera g_camera; // global camera object

// set camera matrix to identity
void init_camera(mcamera *camera);

// update location and rotation of a camera
void position_camera(mcamera *camera, float x, float y, float z, float pt, float yw, float rl);

// set the lookat vector of the camera
void set_cam_lookat(mcamera *camera, fvec eye, fvec lookat);

#endif