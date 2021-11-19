/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/
#ifndef _SKYBOX_H
#define _SKYBOX_H

#ifndef _TRIANGLE_H
#include "triangle.h"
#endif

void draw_skybox(uint32 *txtrs[6], uint16 w, uint16 h, fmatrix *fr, fmatrix *cam);


#endif