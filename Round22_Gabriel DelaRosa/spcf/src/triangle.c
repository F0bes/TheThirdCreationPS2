/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/
#include "triangle.h"

int32 tri_alpha=0;
uint32 *tri_txtr=0;
int32 tri_txtr_w=0;
int32 tri_txtr_h=0;
int32 tri_light=0;
int32 tri_aa=0;
int32 tri_cull=0;

fvec tri_light_pos;

#define clampc(n) {if (n<0) n=0; else if (n>255) n=255;}

// remove backfaces
int backface_cull(vertex *tri)
{
	fvec vec1,vec2,vec3,los;
	float dotp;

	if (!tri)// || !cam)
		return;

	vec1.x = tri[1].v.x - tri[0].v.x;
	vec1.y = tri[1].v.y - tri[0].v.y;
	vec1.z = tri[1].v.z - tri[0].v.z;
	vec1.w = 0;
	vec2.x = tri[2].v.x - tri[0].v.x;
	vec2.y = tri[2].v.y - tri[0].v.y;
	vec2.z = tri[2].v.z - tri[0].v.z;
	vec2.w = 0;
	crossprod(vec1, vec2, &vec3);
	norm(&vec3);
	los.x = 0;//cam->xx - tri[0].v.x;
	los.y = 0;//cam->yy - tri[0].v.y;
	los.z = -1;//cam->zz - tri[0].v.z;
	//norm(&los);
	dotp = los.x*vec3.x + los.y*vec3.y + los.z*vec3.z;

	if (dotp >=0)
	{
		return 1;
	}

	return 0;
}

//render a single triangle after clipping,  can be textured, alpha-blended, and diffuse lit
void render_tri(vertex *tri, fmatrix *g_fr, fmatrix *g_worldtocam)
{
	int16 i,  x, y;
	vertex trfv[16], *clpv, *pv;
	uint32 z;
	int usetxtr = 0;


	if (tri_txtr)
		usetxtr = 1;

	//transform all 3 tri verts to camera space using VU0
	transformf4(g_worldtocam, tri, &trfv[0], 3);

	//copy s,t,r,q, and n from tri to trfv
	for (i=0; i<3; i++)
	{
		trfv[i].s = tri[i].s;
		trfv[i].t = tri[i].t;
		trfv[i].r = tri[i].r;
		trfv[i].q = tri[i].q;
		trfv[i].n = tri[i].n;
	}
	// do not render a backface
	if (tri_cull)
	{
		if (backface_cull(trfv))
			return;
	}

	// clip the verts to the near plane
	clpv = &trfv[8];
	gCP = 3;
	gCP = clip(CLIP_FR, clpv, trfv, -1.0f);
	if (gCP < 3)
		return;

	// transform to homogeneous clip space and perform homogeneous divide
	for (i=0; i<gCP; i++)
	{
		clpv[i].v.w = 1.0f;

		transformf4(g_fr, &clpv[i], &trfv[i], 1);//multiply with projection mtx using VU0
		trfv[i].v.x /= trfv[i].v.w;
		trfv[i].v.y /= trfv[i].v.w;
		trfv[i].v.z /= trfv[i].v.w;
		if (usetxtr) // if using texture, divide tex coords
		{
			trfv[i].s = clpv[i].s / trfv[i].v.w;
			trfv[i].t = clpv[i].t / trfv[i].v.w;
			trfv[i].r = trfv[i].q = 0;
		}else // just copy tex coords
		{
			trfv[i].s = clpv[i].s;
			trfv[i].t = clpv[i].t;
			trfv[i].r = clpv[i].r;
			trfv[i].q = clpv[i].q;
		}
		trfv[i].v.w = 1.0f / trfv[i].v.w;

		trfv[i].n = clpv[i].n; // copy normal
	}

	// clip left
	gCP = clip(CLIP_LF, clpv, trfv, -1.0f);
	if (gCP < 3)
		return;

	// clip right
	gCP = clip(CLIP_RT, trfv, clpv, 1.0f);
	if (gCP < 3)
		return;

	// clip top
	gCP = clip(CLIP_TP, clpv, trfv, 0.7f);
	if (gCP < 3)
		return;

	// clip bottom
	gCP = clip(CLIP_BT, trfv, clpv, -0.7f);
	if (gCP < 3)
		return;

	pv = trfv;

	BEGIN_GS_PACKET(gs_dma_buf);
	GIF_TAG_AD(gs_dma_buf, (gCP*(3+tri_alpha))+6, 1, 0, 0, 0);

	// Access the TEXFLUSH register with anything
	GIF_DATA_AD(gs_dma_buf, texflush, 0x42);

	// set number of mip levels ,filtering, and LOD
	GIF_DATA_AD(gs_dma_buf, tex1_1, GS_TEX1(0,0,1,5,0,0,0));

	//clamp textures
	GIF_DATA_AD(gs_dma_buf, clamp_1,
		GS_CLAMP1(1,1,0,0,0,0));

	// clamp colors
	GIF_DATA_AD(gs_dma_buf, colclamp, 1);

	// Setup the Texture Buffer register
	GIF_DATA_AD(gs_dma_buf, tex0_1,
		GS_TEX0(
			g2_texbuf_addr/256,	// base pointer
			tri_txtr_w / 64,	            // width
			0,					// 32bit RGBA
			gs_texture_wh(tri_txtr_w),	// width
			gs_texture_wh(tri_txtr_h),	// height
			1,					// RGBA
			TEX_DECAL,			// just overwrite existing pixels
			0,0,0,0,0));


	// setup a triangle fan prim
	GIF_DATA_AD(gs_dma_buf, prim,
		GS_PRIM(PRIM_TRI_FAN, 1, usetxtr, 0, tri_alpha, tri_aa, 0, 0, 0));

	//setup each vertex
	for (i=0; i<gCP; i++)
	{
		// transform to screen coords
		x = (int16) (((pv[i].v.x+1.0f) * SCR_W/2) + .5f);
		y = (int16) (SCR_H - (((pv[i].v.y+1.0f) * SCR_H/2) + .5f));
		z = (uint32)(g_zmax - (uint32)(pv[i].v.z * g_zmax + 0.5f));

		x += g2_origin_x;
		y += g2_origin_y;

		GIF_DATA_AD(gs_dma_buf, st,    GS_ST(fptosi2(trfv[i].s), fptosi2(trfv[i].t) ));

		if (tri_light)
		{
			//calc diffuse light, N dot L
			float r,g,b,dotp;
			dotp = pv[i].n.x*tri_light_pos.x + pv[i].n.y*tri_light_pos.y + pv[i].n.z*tri_light_pos.z;
			dotp *= 128;
			dotp -= 32; // light intensity - TODO: make variable
			r = pv[i].s + dotp;  clampc(r);
			g = pv[i].t + dotp;  clampc(g);
			b = pv[i].r + dotp;  clampc(b);
			GIF_DATA_AD(gs_dma_buf, rgbaq,
				GS_RGBAQ((uint8)r, (uint8)g, (uint8)b, 0x80, fptosi2(pv[i].v.w)));
		}else
		{
			GIF_DATA_AD(gs_dma_buf, rgbaq,
				GS_RGBAQ(g2_col_r, g2_col_g, g2_col_b, 0x80, fptosi2(pv[i].v.w)));
		}

		if (tri_aa)
		{
			GIF_DATA_AD(gs_dma_buf, alpha_1, GS_ALPHA(0,1,0,1,0));
		}else
		if (tri_alpha)
		{
			GIF_DATA_AD(gs_dma_buf, alpha_1, GS_ALPHA(0,2,0,1,0));//Cs,0,As,Cd,0
		}
		GIF_DATA_AD(gs_dma_buf, xyz2,  GS_XYZ2(x<<4, y<<4, z));
	}

	SEND_GS_PACKET(gs_dma_buf);
}
