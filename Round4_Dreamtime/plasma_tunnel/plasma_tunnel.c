//------------------------------------------------------------------------
// File:	plasma_tunnel.c
// Author:	Tony Saveski, t_saveski@yahoo.com (aka Dreamtime)
// Note:	Algorithm & Bitmap by Jan Horn, http://www.sulaco.ca.za
//------------------------------------------------------------------------
#include <GL/gl.h>
#include <GL/dgl.h>
#include <GL/dgl_font.h>
#include <math.h>

extern unsigned int tunnel[];
extern unsigned int tunnel_w;
extern unsigned int tunnel_h;

DGLcontext	ctx;

//------------------------------------------------------------------------
#define MY_PI 	3.14259
#define MAX_I	32
#define MAX_J	16

typedef struct
{
	GLfloat x, y, z;
} coord_t;

typedef struct
{
	GLfloat u, v;
} texcoord_t;

coord_t		vert[MAX_I+1][MAX_J+1];
texcoord_t	texcoord[MAX_I+1][MAX_J+1];
GLfloat		angle;
GLuint		tname;


//------------------------------------------------------------------------
int init(void)
{
	memset(&ctx, 0, sizeof(DGLcontext));

	ctx.ScreenWidth		= 512;
	ctx.ScreenHeight	= 224;
	ctx.ScreenDepth		= 32;
	ctx.ScreenBuffers	= 2;
	ctx.ZDepth			= 32;

	if(*((char *)0x1FC80000 - 0xAE) == 'E')
		ctx.ps2VidType		= PS2_PAL;
	else
		ctx.ps2VidType		= PS2_NTSC;

	ctx.ps2VidInterlace	= PS2_NONINTERLACED;
	ctx.ps2VidFrame		= PS2_FRAME;

	return(dglInit(&ctx));
}

//------------------------------------------------------------------------
void create_tunnel(void)
{
int	i, j;

	for(i=0; i<=MAX_I; i++)
	{
		for(j=0; j<=MAX_J; j++)
		{
			vert[i][j].x = (3.0 - j/12.0)*cos(2.0*MY_PI/32.0*i);
			vert[i][j].y = (3.0 - j/12.0)*sin(2.0*MY_PI/32.0*i);
			vert[i][j].z = -j*3;
		}
	}
}

//------------------------------------------------------------------------
void draw(void)
{
int 	i, j;
GLfloat	c;

	angle += 4.0;

	for(i=0; i<=MAX_I; i++)
	{
		for(j=0; j<=MAX_J; j++)
		{
			texcoord[i][j].u = i/32.0 + cos((angle + 8.0*j)/60.0)/2.0;
			texcoord[i][j].v = j/32.0 + (angle + j)/120.0;
		}
	}

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, tname);

	for(j=0; j<MAX_J; j++)
	{
		if(j>MAX_J/2) c = 1.0-(j-MAX_J/2)/10.0;
		else c = 1.0;

		glColor3f(c, c, c);

		glBegin(GL_TRIANGLES);
		for(i=0; i<MAX_I; i++)
		{
			glTexCoord2f(texcoord[i][j].u, texcoord[i][j].v);
			glVertex3f(vert[i][j].x, vert[i][j].y, vert[i][j].z);

			glTexCoord2f(texcoord[i+1][j].u, texcoord[i+1][j].v);
			glVertex3f(vert[i+1][j].x, vert[i+1][j].y, vert[i+1][j].z);

			glTexCoord2f(texcoord[i+1][j+1].u, texcoord[i+1][j+1].v);
			glVertex3f(vert[i+1][j+1].x, vert[i+1][j+1].y, vert[i+1][j+1].z);

			glTexCoord2f(texcoord[i][j].u, texcoord[i][j].v);
			glVertex3f(vert[i][j].x, vert[i][j].y, vert[i][j].z);

			glTexCoord2f(texcoord[i+1][j+1].u, texcoord[i+1][j+1].v);
			glVertex3f(vert[i+1][j+1].x, vert[i+1][j+1].y, vert[i+1][j+1].z);

			glTexCoord2f(texcoord[i][j+1].u, texcoord[i][j+1].v);
			glVertex3f(vert[i][j+1].x, vert[i][j+1].y, vert[i][j+1].z);
		}
		glEnd();
	}

	dglFontPrint(8, ctx.ScreenHeight-20, "PLASMA TUNNEL by Dreamtime!                            ");
	dglFontPrint(8, ctx.ScreenHeight-34, "Algorithm & Bitmap by Jan Horn, http://www.sulaco.ca.za");

	glFlush();
}

//------------------------------------------------------------------------
int main(void)
{
	sif_rpc_init(0);

    if(!init()) return(1);
	dglFontInit();
	dglFontCharSize(14, 14);
	dglFontCharSpace(-5);

    create_tunnel();
    angle = 0.0;

	glShadeModel(GL_FLAT);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_TEXTURE_2D);
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glGenTextures(1, &tname);
	glBindTexture(GL_TEXTURE_2D, tname);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, tunnel_w, tunnel_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tunnel);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 100.0);

	while(1)
	{
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -2.0f);

		draw();
		dglSwapBuffers();
	}

	dglFontEnd();
	dglEnd();
	return(0);
}
