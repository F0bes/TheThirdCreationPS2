/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/
#ifndef _PARTICLE_H
#define _PARTICLE_H

#ifndef __DATATYPES_
#include "types.h"
#endif
#ifndef _TRIANGLE_H
#include "triangle.h"
#endif
#include "matrix.h"

#define MAX_JET_PARTICLES 100
#define MAX_PARTICLESHAPES 10

typedef struct
{
	int	   active;	
	float  fSumTime;		
	float  fade;		
	fvec   ptPos;
	fvec   ptVel;
}PARTICLE;

typedef struct
{
	fvec  ptOrigin;
	int   dRender;
	float particle_size;
	uint32 *txtr;
	uint16 txtr_w;
	uint16 txtr_h;
	vertex quad[4];
	PARTICLE JetArr[MAX_JET_PARTICLES];

} PARTICLE_JET;

typedef struct poly
{
	vertex vp[4];
	float dist;
	struct poly *prev;
	struct poly *next;
} POLY;

#define POLYLIST POLY*

void init_jet_particle(PARTICLE_JET *pJet,int idx);
void create_jet(PARTICLE_JET *pJet);
void reset_jet(PARTICLE_JET *pJet);
void end_jet(PARTICLE_JET *pJet);
void tick_jet(PARTICLE_JET *pJet);
void set_quad(PARTICLE_JET *pJet);
void draw_jet(PARTICLE_JET *pJet, fmatrix *cam, fmatrix *fr, fmatrix *worldtocam);
#endif