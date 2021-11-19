/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/
#include "particle.h"
#include "math.h"
#include <stdlib.h>

extern fmatrix *g_fr;
extern fmatrix *g_worldtocam;

//--------------------------------------------------------------
void reset_jet(PARTICLE_JET *pJet)
{
	int i = 0;

	for (i=0; i < MAX_JET_PARTICLES; i++)
		init_jet_particle(pJet,i);

	pJet->dRender = 0;
}

//--------------------------------------------------------------
void init_jet_particle(PARTICLE_JET *pJet,int idx)
{
	PARTICLE *JetArr = pJet->JetArr;

	JetArr[idx].active = 1;								
	JetArr[idx].fSumTime = 0.0f;
	JetArr[idx].fade =    (float)(((uint32)rand())%100)/1000.0f+0.003f;
	JetArr[idx].ptVel.x = 0;//((float)(((uint32)rand())%12)-6.0f)*.0001f;//1200 600
	JetArr[idx].ptVel.y = 0;//((float)(((uint32)rand())%12)-6.0f)*.001f;
	JetArr[idx].ptVel.z = -((float)(((uint32)rand())%3)-0.0f)*.005f;
	JetArr[idx].ptPos.x = 0;//(float)((((uint32)rand())%2)-1.0f)*.001;//8 4
	JetArr[idx].ptPos.y = 0;//(float)((((uint32)rand())%2)-1.0f)*.001;//8 4
	JetArr[idx].ptPos.z = 0;//(float)((((uint32)rand())%2)-1.0f)*.0001;//8 */
}

//-------------------------------------------------------
void tick_jet(PARTICLE_JET *pJet)
{
	float max_time = .09f;
	float alpha; 
	int i;
	PARTICLE *JetArr = pJet->JetArr;

	for(i=0; i<MAX_JET_PARTICLES; i++)
	{
		if (JetArr[i].active)
		{
			if (JetArr[i].fSumTime > max_time)
			{
				JetArr[i].active = 0;
				JetArr[i].fSumTime = 1.0f;
			}
			alpha = 1.0f-JetArr[i].fSumTime/max_time;

			JetArr[i].ptPos.x += JetArr[i].ptVel.x;
			JetArr[i].ptPos.y += JetArr[i].ptVel.y;
			JetArr[i].ptPos.z += JetArr[i].ptVel.z;

			JetArr[i].fSumTime += JetArr[i].fade;
		}
		else
		{
			init_jet_particle(pJet,i);
		}
	}
}
// create the four corners of particle sprite
//------------------------------------------------------------------
void set_quad(PARTICLE_JET *pJet)
{
	pJet->quad[0].v.x = -pJet->particle_size;
	pJet->quad[0].v.y = pJet->particle_size;
	pJet->quad[0].v.z = 0;
	pJet->quad[0].v.w = 0;
	pJet->quad[0].s = 0;
	pJet->quad[0].t = 0;
	pJet->quad[1].v.x = pJet->particle_size;
	pJet->quad[1].v.y = pJet->particle_size;
	pJet->quad[1].v.z = 0;
	pJet->quad[1].v.w = 0;
	pJet->quad[1].s = 1;
	pJet->quad[1].t = 0;
	pJet->quad[2].v.x = -pJet->particle_size;
	pJet->quad[2].v.y = -pJet->particle_size;
	pJet->quad[2].v.z = 0;
	pJet->quad[2].v.w = 0;
	pJet->quad[2].s = 0;
	pJet->quad[2].t = 1;
	pJet->quad[3].v.x = pJet->particle_size;
	pJet->quad[3].v.y = -pJet->particle_size;
	pJet->quad[3].v.z = 0;
	pJet->quad[3].v.w = 0;
	pJet->quad[3].s = 1;
	pJet->quad[3].t = 1;
}

//---------------------------------------------------------------------
POLYLIST insert(POLYLIST plist, POLY *poly, fmatrix *cam)
{
	float xd,yd,zd;
	POLY *iter, *tail;
	if (!poly || !cam)
		return plist;

	xd = poly->vp[0].v.x - cam->xx;
	yd = poly->vp[0].v.y - cam->yy;
	zd = poly->vp[0].v.z - cam->zz;
	poly->dist = sqrt(xd*xd + yd*yd + zd*zd);

	//if empty list, make poly list head
	if (!plist)
	{
		plist = poly;
		plist->next = plist-> prev = 0;
		return plist;
	}
	// search list
	iter = plist;
	tail = plist;
	while(iter)
	{
		//insert before iter
		if (poly->dist > iter->dist)
		{
			POLY *prevp = iter->prev;
			if (prevp)
			{
				prevp->next = poly;
				poly->prev = prevp;
				poly->next = iter;
				iter->prev = poly;
			}
			return plist;
		}
		tail = iter;
		iter = iter->next;
	}
	// insert as tail
	if (tail)
	{
		tail->next = poly;
		poly->prev = tail;
		poly->next = 0;
		return plist;
	}
}

//----------------------------------------------------------------------
void draw_jet(PARTICLE_JET *pJet, fmatrix *cam, fmatrix *fr, fmatrix *worldtocam)
{
	int i,j;
	fmatrix mat;
	vertex vp[4];
	PARTICLE *JetArr = pJet->JetArr;
	POLYLIST poly_list=0;
	POLY *newp=0;
	POLY pool[MAX_JET_PARTICLES];
	int idx = 0;

	if (pJet->dRender == 0)
		return;

	// create rotation matrix using camera
	identitymatrixf(&mat);
	mat.xu = cam->xu;  mat.xv = cam->xv;  mat.xw = -cam->xw;
	mat.yu = cam->yu;  mat.yv = cam->yv;  mat.yw = -cam->yw;
	mat.zu = cam->zu;  mat.zv = cam->zv;  mat.zw = -cam->zw;

	// setup sprite texture, turn on alpha blending
	tri_txtr = pJet->txtr;
	tri_txtr_w = pJet->txtr_w;
	tri_txtr_h = pJet->txtr_h;
	tri_alpha = 1;
	tri_light = 0;
	tri_aa = 0;
	tri_cull=0;
	// load texture to GS
	gs_load_texture(0, 0, pJet->txtr_w, pJet->txtr_w, pJet->txtr, g2_texbuf_addr, pJet->txtr_w);

	// create an ordered list of particle sprites
	for(i=0; i<MAX_JET_PARTICLES; i++)
	{
		if (JetArr[i].active)
		{
			//newp = (POLY*)malloc(sizeof(POLY));
			//if (!newp)
			//	continue;
			newp = &pool[idx++];
			transformf4(&mat, &pJet->quad[0], &newp->vp[0], 4);//orient to camera rotation using VU0
			for (j=0; j<4; j++)
			{
				// translate to world pos and copy tex coords
				newp->vp[j].v.x += JetArr[i].ptPos.x + pJet->ptOrigin.x;
				newp->vp[j].v.y += JetArr[i].ptPos.y + pJet->ptOrigin.y;
				newp->vp[j].v.z += JetArr[i].ptPos.z + pJet->ptOrigin.z;
				newp->vp[j].s = pJet->quad[j].s;
				newp->vp[j].t = pJet->quad[j].t;
				newp->vp[j].v.w = 1.0f;
			}
			poly_list = insert(poly_list, newp, cam);
			//render each triangle of sprite quad
			//render_tri(&vp[0], fr, worldtocam);
			//render_tri(&vp[1], fr, worldtocam);
		}
	}

	// render and free list
	while(poly_list)
	{
		POLY *poly = poly_list;
		render_tri(&poly->vp[0], fr, worldtocam);
		render_tri(&poly->vp[1], fr, worldtocam);
		poly_list = poly->next;
		//free(poly);
		if (!poly_list)
			break;
	}

	//pJet->dRender = 0;
}
