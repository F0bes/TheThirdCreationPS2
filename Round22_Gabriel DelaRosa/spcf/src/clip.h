/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/
#ifndef _CLIP_H
#define _CLIP_H

#ifndef __DATATYPES_
#include "types.h"
#endif


enum {CLIP_LF=0, CLIP_RT, CLIP_BT, CLIP_TP, CLIP_NR, CLIP_FR};

int clip(int plane, vertex *clipped_pts,vertex *pts, float limit);

extern int gCP;


#endif