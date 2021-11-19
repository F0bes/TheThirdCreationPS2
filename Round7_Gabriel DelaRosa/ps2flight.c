//---------------------------------------------------------------------------
// File:	Ps2flight.c
// Author:	Gabriel T. DelaRosa
// Note:  Most of this code I copied from Dreamtime's tutorials, the Turnip
//        game, and the Funslower demo.  I added the DrawHorizon function,
//        the camera functions, and the main loop code.
//---------------------------------------------------------------------------
#include <stdlib.h> 	// Feels weird having these :)
#include <stdio.h>		// Thanks GUSTAVO!
#include <string.h>
#include <tamtypes.h>
#include <sifcmd.h>
#include <kernel.h>
#include <sifrpc.h>

#include "g2.h"
#include "gs.h"

#include "math.h"
#include "matrix.h"
#include "clip.h"

//------thanks Pukko
#include "hw.h"
#include "pad.h"
#include "loadmodule.h"

extern char padBuf[];
struct padButtonStatus buttons;
int ret;
int port, slot;
int i;
u32 paddata;
u32 old_pad = 0;
u32 new_pad;
//--------

/*extern uint32 hover_w;
extern uint32 hover_h;
extern uint32 hover[];
extern uint32 message_w;
extern uint32 message_h;
extern uint32 message[];*/

// font
//extern uint32 courier_new[];
//extern uint16 fixed_tc[];

extern uint32 stone_w;
extern uint32 stone_h;
extern uint32 stone[];
extern uint32 stone1_w;
extern uint32 stone1_h;
extern uint32 stone1[];
extern uint32 stone2_w;
extern uint32 stone2_h;
extern uint32 stone2[];
extern uint32 hrntpnl_w;
extern uint32 hrntpnl_h;
extern uint32 hrntpnl[];
extern uint32 clouds_w;
extern uint32 clouds_h;
extern uint32 clouds[];

static uint16 maxx, maxy;

static fmatrix g_cam;
static float g_loc[3];
static float g_rot[3];
static float SCR_W;
static float SCR_H;
static float SCR_CEN_X;
static float SCR_CEN_Y;
static float VIEW_DISTANCE;



void DrawHorizon(float ydiff,float ylim)
{
	fmatrix viewmtx;
	fvec trfpt;
	float scale,mag;
	uint16 cp,i;
	fvec clipped_vpts[8], vpts[8];
	fvec pt = {-SCR_W/2, SCR_H/2, -VIEW_DISTANCE, 0.0f};
	float s,t;

	if (ydiff == 0)
		return;


	// UL
	transformf3(&g_cam,&pt,&vpts[0],1);
	// UR
	pt.x*=-1; // make x pos
	transformf3(&g_cam,&pt,&vpts[1],1);
	//LR  
	pt.y*=-1; // make y neg
	transformf3(&g_cam,&pt,&vpts[2],1);
	//LL  
	pt.x*=-1; // make x,y neg
	transformf3(&g_cam,&pt,&vpts[3],1);


	if (ydiff < 0)
		cp = clip_above(clipped_vpts,vpts,ylim);
	else
		cp = clip_below(clipped_vpts,vpts,-ylim);

	if (cp == 0)
	{
		return;
	}


	setviewmatrixf(&g_cam,&viewmtx);


	if (ydiff >= 0)
	g2_begin_trifan((int32)cp,stone_w,stone_h,stone,stone1,stone2);
	else
	g2_begin_trifan((int32)cp,clouds_w,clouds_h,clouds,0,0);
	for (i=0; i < cp; i++)
	{
		pt.x = clipped_vpts[i].x;
		pt.y = clipped_vpts[i].y;
		pt.z = clipped_vpts[i].z;
		pt.w = 1.0f;
		// normalize pt
		mag = (float)sqrt(pt.x*pt.x + pt.y*pt.y + pt.z*pt.z);
		pt.x /= mag;
		pt.y /= mag;
		pt.z /= mag;

		// scale pt by altitude
		scale = (pt.y == 0 ? 0 : ydiff / pt.y);
		if (scale < 0)
			scale *= -1;
		pt.x *= scale;
		pt.y *= scale;
		pt.z *= scale;

		// translate to camera pos
		pt.x += g_cam.xx;
		pt.y += g_cam.yy;
		pt.z += g_cam.zz;

		// texture coords
		if (ydiff >= 0)
		{
		s = (20000.0f + pt.x) / 10000.0f;
		t = (20000.0f + pt.z) / 10000.0f;
		}
		else
		{
			s = (20000.0f + pt.x) / 3000.0f;
			t = (20000.0f + pt.z) / 3000.0f;
		}

		// transform to camera space
		transformf(&viewmtx,&pt,&trfpt,1);

		//project to 2D
		pt.x = SCR_CEN_X + ((trfpt.x/-trfpt.z) * VIEW_DISTANCE);
		pt.y = SCR_CEN_Y - ((trfpt.y/-trfpt.z) * VIEW_DISTANCE);
		pt.z = -trfpt.z;
		g2_add_pt_trifan(fptosi(pt.x), fptosi(pt.y), pt.z, s/pt.z, t/pt.z, 1.0f/pt.z);
	}

	g2_end_trifan();

}


//---------------------------------------------------------------------------
#define VFREQ	50
void crap_delay(float s)
{
int i;

	for(i=0; i<s*VFREQ; i++) g2_wait_vsync();
}
//---------------------------------------------------------------------------
void flip_buffers(void)
{
	g2_wait_vsync();
	g2_set_visible_frame(1 - g2_get_visible_frame());
	g2_set_active_frame(1 - g2_get_active_frame());
}

//---------------------------------------------------------------------------
// PAD RELATED STUFF
//   Yeh...I KNOW GLOBALS ARE BAD...mmmkay!
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void pad_initialize(void)
{
//---thanks Pukko
	sif_rpc_init(0);
	install_VRstart_handler();
	loadModules();
	padInit(0);
	port = 0;
	slot = 0;
	if((ret = padPortOpen(0, 0, padBuf)) == 0)
	{
		scr_printf("padOpenPort failed: %d\n", ret);
		k_SleepThread();
	}

	if(!initializePad(0, 0))
	{
		scr_printf("pad initalization failed!\n");
		k_SleepThread();
	}
//--------
}

//---------------------------------------------------------------------------
void pad_update(void)
{
//---thanks Pukko
	while(padGetState(port, slot) != PAD_STATE_STABLE)
	{
		WaitForNextVRstart(1); // Perhaps a bit to long ;)
	}

	ret = padRead(port, slot, &buttons); // port, slot, buttons
	if(ret != 0)
	{
		paddata = 0xffff ^ ((buttons.btns[0] << 8) | buttons.btns[1]);
		new_pad = paddata & ~old_pad;
		old_pad = paddata;
	}
//--------
}

//---------------------------------------------------------------------------
int pad_pressed(uint32 button)
{
//---thanks Pukko
	if(new_pad & button) return(1);
	return(0);
//--------
}

//---------------------------------------------------------------------------
void pad_wait_button(uint32 button)
{
	while(1)
	{
		pad_update();
		if(new_pad & button) return;
	}
}

//---------------------------------------------------------------------------
/*void scroll_background(void)
{
int y, i;

	for(y=-hover_h; y<=0; y+=2)
	{
		g2_put_image(0, y, hover_w, hover_h, hover);
		flip_buffers();
	}
	for(i=0; i<0x1FFFFFF; i++) {} // delay
}

//---------------------------------------------------------------------------
void scroll_message(void)
{
uint32 i;
uint16 x, y;

	g2_put_image(0, 0, hover_w, hover_h, hover);
	g2_put_image(10, 10, message_w, message_h, message);
	flip_buffers();
	for(i=0; i<0x4FFFFFF; i++) {} // delay

	// Go Left to Right
	for(x=10; x<maxx-message_w-10; x+=4)
	{
		g2_put_image(0, 0, hover_w, hover_h, hover);
		g2_put_image(x, 10, message_w, message_h, message);
		flip_buffers();
	}


	// Go Top to Bottom
	for(y=10; y<maxy-message_h-10; y+=2)
	{
		g2_put_image(0, 0, hover_w, hover_h, hover);
		g2_put_image(x, y, message_w, message_h, message);
		flip_buffers();
	}


	// Go Right to Left
	for(; x>10; x-=4)
	{
		g2_put_image(0, 0, hover_w, hover_h, hover);
		g2_put_image(x, y, message_w, message_h, message);
		flip_buffers();
	}


	// Go Bottom to Top
	for(; y>10; y-=2)
	{
		g2_put_image(0, 0, hover_w, hover_h, hover);
		g2_put_image(x, y, message_w, message_h, message);
		flip_buffers();
	}

	for(i=0; i<0x4FFFFFF; i++) {} // delay

	g2_put_image(0, 0, hover_w, hover_h, hover);
	flip_buffers();
	g2_put_image(0, 0, hover_w, hover_h, hover);
	flip_buffers();
}*/


void _setmatrixf(fmatrix *m, float *rot) //need ?
{
	float cr,sr,cp,sp,cy,sy,srsp,crsp;

	if (!rot || !m)
		return;

	identitymatrixf(m);

	cr = cos(rot[0]*.01745f);
	sr = sin(rot[0]*.01745f);

	cp = cos(rot[1]*.01745f);
	sp = sin(rot[1]*.01745f);

	cy = cos(rot[2]*.01745f);
	sy = sin(rot[2]*.01745f);


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


//----------------------------------------------------------------------------
void init_cam()
{
	identitymatrixf(&g_cam);
	g_loc[0] = 0.0f;
	g_loc[1] = 7.0f;
	g_loc[2] = -0.00002f;
	g_rot[0] = 0.0f;
	g_rot[1] = 225.0f;
	g_rot[2] = 0.0f;
	_setmatrixf(&g_cam,g_rot);
	movematrixf(&g_cam,g_loc[0],g_loc[1],g_loc[2]);
	g_rot[1] = 0.0f;
	g_loc[1] = 0.0f;
}

//----------------------------------------------------------------------------
void move_cam()
{
	float x,z;

	g_cam.xx +=g_cam.xu*g_loc[0] + g_cam.xv*g_loc[1] + g_cam.xw*g_loc[2];
	g_cam.yy +=g_cam.yu*g_loc[0] + g_cam.yv*g_loc[1] + g_cam.yw*g_loc[2];
	g_cam.zz +=g_cam.zu*g_loc[0] + g_cam.zv*g_loc[1] + g_cam.zw*g_loc[2];

	x = g_cam.xx;
	z = g_cam.zz;
	if (x < 0)
		x = -x;
	if (z < 0)
		z = -z;
	if (x > 1000)
	{
		float qt = x / 1000;
		float xdiff = x - (qt*1000);
		g_cam.xx = g_cam.xx > 0.0f ? xdiff - 1000 :  1000 - xdiff;
	}
	if (z > 1000)
	{
		float qt = z / 1000;
		float zdiff = z - (qt*1000);
		g_cam.zz = g_cam.zz > 0.0f ? zdiff - 1000 : 1000 - zdiff;
	}

	if (g_cam.yy <= .5f)
		g_cam.yy = .5f;
	else
	if (g_cam.yy > 40.0f)
		g_cam.yy = 40.0f;
}

//---------------------------------------------------------------------------
int main(int argc, char **argv)
{
	float ail_pos, elev_pos;
	fmatrix tmp_rot;

	argc=argc;
	argv=argv;

	if(gs_is_ntsc())
	{
		g2_init(NTSC_512_224_32);
		SCR_W = 512.0f;
		SCR_H = 224.0f;
		VIEW_DISTANCE = 443.506;
	}
	else
	{
		g2_init(PAL_512_256_32);
		SCR_W = 512.0f;
		SCR_H = 256.0f;
		VIEW_DISTANCE = 443.506;
	}

	// You'd be STUPID if you put this before the g2_init(), right? :-)
	maxx = g2_get_max_x();
	maxy = g2_get_max_y();

	// clear the screen
	g2_set_fill_color(0, 0, 0);
	g2_fill_rect(0, 0, maxx, maxy);

	// draw next frame to hidden buffer
	g2_set_active_frame(1);
	g2_set_fill_color(0, 0, 0);
	g2_fill_rect(0, 0, maxx, maxy);

	//initialize screen info
	SCR_CEN_X = SCR_W / 2;
	SCR_CEN_Y = SCR_H / 2;

	init_cam();

	pad_initialize();

	while(1)
	{
		//scroll_background();
		//scroll_message();
		g2_set_fill_color(80, 128, 200);
		g2_fill_rect(0, 0, maxx, maxy);

		pad_update();

		elev_pos = ((buttons.rjoy_v - 128) * .005f);
		if (elev_pos > -.2 && elev_pos < .2)
			elev_pos = 0;
		g_rot[0] = elev_pos;

		ail_pos = ((buttons.rjoy_h - 128) * -.0098f);
		if (ail_pos > -.2 && ail_pos < .2)
			ail_pos = 0;
		g_rot[2] = ail_pos;

		_setmatrixf(&tmp_rot,g_rot);
		fmulmatrixf(&g_cam,&tmp_rot);

		g_loc[2] = -.8f;
		move_cam();

//		g2_set_color(0,32,64);//130, 60, 28);
		DrawHorizon(g_cam.yy,1.0f);
		DrawHorizon(-38.0f,75.0f);
		

		g2_put_image(0,0,hrntpnl_w,hrntpnl_h,hrntpnl);

		flip_buffers();
	}

	// ok...it will never get here...
	g2_end();
	return(0);
}

