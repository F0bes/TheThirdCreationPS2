/* Copyright (C) Gabriel T. DelaRosa, 2004. 
 * All rights reserved worldwide.
*/

#include "clip.h"
#ifndef __my_math
#include "math.h"
#endif

int gCP = 0;


//----------------------------------------------------------------
int clip(int plane, vertex *clipped_pts,vertex *pts, float limit)
{
	int X, Y, Z, side;
	int v1 = gCP-1;
	int v2 = 0;
	int cp = 0;
	float scale, *pClipv, *pP1v, *pP2v;

	// determine coordinate offsets according to clip plane, 
	switch(plane)
	{
		case CLIP_LF :  
		case CLIP_RT :  X=1; Y=2; Z=0; break;
		case CLIP_BT :  
		case CLIP_TP :  X=2; Y=0; Z=1; break;
		case CLIP_NR :  
		case CLIP_FR :  X=0; Y=1; Z=2; break;
	};
	// compare above or below?
	side = plane % 2;

	// starting with last point,  check if the point or its next adjacent neighbor should be clipped
	for (v2=0; v2 < gCP; v2++)
	{
		pP1v = &pts[v1].v.x;
		pP2v = &pts[v2].v.x;

		//both inside?
		if ((side == 0 && *(pP1v+Z) >= limit && *(pP2v+Z) >= limit) ||
			(side != 0 && *(pP1v+Z) <= limit && *(pP2v+Z) <= limit))
		{
			// copy pt2 to clipped pts
			clipped_pts[cp  ].v  = pts[v2].v;
			clipped_pts[cp  ].s  = pts[v2].s;
			clipped_pts[cp  ].t  = pts[v2].t;
			clipped_pts[cp  ].r  = pts[v2].r;
			clipped_pts[cp  ].q  = pts[v2].q;
			clipped_pts[cp++].n  = pts[v2].n;
		}
		else
		if ((side == 0 && *(pP1v+Z) < limit && *(pP2v+Z) < limit) ||
			(side != 0 && *(pP1v+Z) > limit && *(pP2v+Z) > limit))
		{
			// both outside - do nothing
		}
		else 
		{
			pClipv = &clipped_pts[cp].v.x;
			scale = (limit - *(pP1v+Z)) / (*(pP2v+Z)-*(pP1v+Z));
			// pt1 inside, pt2 outside ?
			if ((side == 0 && *(pP1v+Z) >= limit && *(pP2v+Z) < limit) ||
				(side != 0 && *(pP1v+Z) <= limit && *(pP2v+Z) > limit))
			{
				// clip pt2
				*(pClipv+X) = *(pP1v+X) + ((*(pP2v+X)-*(pP1v+X)) * scale);
				*(pClipv+Y) = *(pP1v+Y) + ((*(pP2v+Y)-*(pP1v+Y)) * scale);
				*(pClipv+Z) = limit;
				clipped_pts[cp  ].s = pts[v1].s + ((pts[v2].s-pts[v1].s) * scale);
				clipped_pts[cp  ].t = pts[v1].t + ((pts[v2].t-pts[v1].t) * scale);
				clipped_pts[cp  ].r   = pts[v1].r + ((pts[v2].r-pts[v1].r) * scale);
				clipped_pts[cp  ].q   = pts[v1].q + ((pts[v2].q-pts[v1].q) * scale);
				clipped_pts[cp  ].n.x = pts[v1].n.x + ((pts[v2].n.x-pts[v1].n.x) * scale);
				clipped_pts[cp  ].n.y = pts[v1].n.y + ((pts[v2].n.y-pts[v1].n.y) * scale);
				clipped_pts[cp  ].n.z = pts[v1].n.z + ((pts[v2].n.z-pts[v1].n.z) * scale);
				clipped_pts[cp++].v.w = pts[v1].v.w + ((pts[v2].v.w-pts[v1].v.w) * scale);
			}else
			// pt1 outside, pt2 inside ?
			if ((side == 0 && *(pP1v+Z) < limit && *(pP2v+Z) >= limit) ||
				(side != 0 && *(pP1v+Z) > limit && *(pP2v+Z) <= limit))
			{
				// clip pt1
				*(pClipv+X) = *(pP1v+X) + ((*(pP2v+X)-*(pP1v+X)) * scale);
				*(pClipv+Y) = *(pP1v+Y) + ((*(pP2v+Y)-*(pP1v+Y)) * scale);
				*(pClipv+Z) = limit;
				clipped_pts[cp  ].s = pts[v1].s + ((pts[v2].s-pts[v1].s) * scale);
				clipped_pts[cp  ].t = pts[v1].t + ((pts[v2].t-pts[v1].t) * scale);
				clipped_pts[cp  ].r   = pts[v1].r + ((pts[v2].r-pts[v1].r) * scale);
				clipped_pts[cp  ].q   = pts[v1].q + ((pts[v2].q-pts[v1].q) * scale);
				clipped_pts[cp  ].n.x = pts[v1].n.x + ((pts[v2].n.x-pts[v1].n.x) * scale);
				clipped_pts[cp  ].n.y = pts[v1].n.y + ((pts[v2].n.y-pts[v1].n.y) * scale);
				clipped_pts[cp  ].n.z = pts[v1].n.z + ((pts[v2].n.z-pts[v1].n.z) * scale);
				clipped_pts[cp++].v.w = pts[v1].v.w + ((pts[v2].v.w-pts[v1].v.w) * scale);

				// copy pt2
				clipped_pts[cp  ].v  = pts[v2].v;
				clipped_pts[cp  ].s  = pts[v2].s;
				clipped_pts[cp  ].t  = pts[v2].t;
				clipped_pts[cp  ].r  = pts[v2].r;
				clipped_pts[cp  ].q  = pts[v2].q;
				clipped_pts[cp++].n  = pts[v2].n;
			}
		}
		v1=v2;
	}
	return cp;
}

