/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/
#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#ifndef G2_H
#include "g2.h"
#endif
#ifndef DMA_H
#include "dma.h"
#endif
#ifndef GS_H
#include "gs.h"
#endif
#ifndef GIF_H
#include "gif.h"
#endif	
#ifndef _CLIP_H
#include "clip.h"
#endif


DECLARE_EXTERN_GS_PACKET(gs_dma_buf);

void render_tri(vertex *tri, fmatrix *g_fr, fmatrix *g_worldtocam);

extern int32 tri_alpha;
extern uint32 *tri_txtr;
extern int32 tri_txtr_w;
extern int32 tri_txtr_h;
extern int32 tri_light;
extern int32 tri_aa;
extern int32 tri_cull;

extern fvec tri_light_pos;


#endif