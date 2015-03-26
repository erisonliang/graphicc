/*
Copyright 2015 William A Adams

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
	References:
		http://joshbeam.com/articles/triangle_rasterization/
*/

#include "raster_rgba.h"

#define swap16(a, b) { int16_t t = a; a = b; b = t; }

typedef struct _point2d
{
	int x;
	int y;
} point2d;

void point2d_init(point2d &pt, const int x, const int y)
{
	pt.x = x;
	pt.y = y;
}

typedef struct _edge
{
	int Color1;
	int Color2;
	int X1, Y1, X2, Y2;
} edge;

void edge_init(edge &e, int x1, int y1, int color1, int x2, int y2, int color2)
{
	e.Color1 = color1;
	e.Color2 = color2;

	e.X1 = x1;
	e.X2 = x2;

	e.Y1 = y1;
	e.Y2 = y2;
}


int FindTopmostPolyVertex(const point2d *poly, const size_t nelems)
{
	int ymin = INT32_MAX;
	int vmin = 0;

	size_t idx = 0;
	while (idx < nelems) {
		if (poly[idx].y < ymin) {
			ymin = poly[idx].y;
			vmin = idx;
		}
		idx++;
	}

	return vmin;
}

void RotateVertices(point2d *res, point2d *poly, const size_t nelems, const int starting)
{
	size_t offset = starting;
	size_t idx = 0;
	while (idx < nelems) {
		res[idx].x = poly[offset].x;
		res[idx].y = poly[offset].y;
		offset++;
		
		if (offset > nelems-1) {
			offset = 0;
		}

		idx++;
	}
}

void sortTriangle(point2d *sorted, const int x1, const int y1, const int x2, const int y2, const int x3, const int y3)
{
	point2d verts[3] = { { x1, y1 }, { x2, y2 }, { x3, y3 } };

	int topmost = FindTopmostPolyVertex(verts, 3);
	RotateVertices(sorted, verts, 3, topmost);
}

void raster_rgba_draw_spans_between_edges(pb_rgba *pb, const edge &e1, const edge &e2)
{
	// calculate difference between the y coordinates
	// of the first edge and return if 0
	float e1ydiff = (float)(e1.Y2 - e1.Y1);
	if (e1ydiff == 0.0f)
		return;

	// calculate difference between the y coordinates
	// of the second edge and return if 0
	float e2ydiff = (float)(e2.Y2 - e2.Y1);
	if (e2ydiff == 0.0f)
		return;

	// calculate differences between the x coordinates
	// and colors of the points of the edges
	float e1xdiff = (float)(e1.X2 - e1.X1);
	float e2xdiff = (float)(e2.X2 - e2.X1);
	int e1colordiff = (e1.Color2 - e1.Color1);
	int e2colordiff = (e2.Color2 - e2.Color1);

	// calculate factors to use for interpolation
	// with the edges and the step values to increase
	// them by after drawing each span
	float factor1 = (float)(e2.Y1 - e1.Y1) / e1ydiff;
	float factorStep1 = 1.0f / e1ydiff;
	float factor2 = 0.0f;
	float factorStep2 = 1.0f / e2ydiff;

	// loop through the lines between the edges and draw spans
	for (int y = e2.Y1; y < e2.Y2; y++) {
		int spanlength = e2.X1 + (int)(e2xdiff * factor2) - e1.X1 + (int)(e1xdiff * factor1);
		// draw the span
		//raster_rgba_draw_span(pb, e1.X1 + (int)(e1xdiff * factor1), e1.Color1 + (e1colordiff * factor1),
		//	e2.X1 + (int)(e2xdiff * factor2), e2.Color1 + (e2colordiff * factor2), y);
		
		
		raster_rgba_hline(pb, e1.X1 + (int)(e1xdiff * factor1), y, spanlength, e1.Color1);

		// increase factors
		factor1 += factorStep1;
		factor2 += factorStep2;
	}
}

void raster_rgba_triangle_fill(pb_rgba *pb, 
	const unsigned int x1, const unsigned int  y1, 
	const unsigned int  x2, const unsigned int  y2, 
	const unsigned int  x3, const unsigned int  y3, 
	int color)
{
	int a, b, y, last;

	// sort vertices, such that 0 == y with lowest number (top)
	point2d sorted[3];
	sortTriangle(sorted, x1, y1, x2, y2, x3, y3);

	// Handle the case where points are colinear (all on same line)
	if (sorted[0].y == sorted[2].y) { 
		a = b = sorted[0].x;
		
		if (sorted[1].x < a) 
			a = sorted[1].x;
		else if (sorted[1].x > b) 
			b = sorted[1].x;

		if (sorted[2].x < a) 
			a = sorted[2].x;
		else if (sorted[2].x > b) 
			b = sorted[2].x;

		raster_rgba_hline(pb, a, sorted[0].y, b - a + 1, color);
		return;
	}

	int16_t
		dx01 = sorted[1].x - sorted[0].x,
		dy01 = sorted[1].y - sorted[0].y,
		dx02 = sorted[2].x - sorted[0].x,
		dy02 = sorted[2].y - sorted[0].y,
		dx12 = sorted[2].x - sorted[1].x,
		dy12 = sorted[2].y - sorted[1].y;
	
	int32_t sa = 0, sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2. If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (sorted[1].y == sorted[2].y) 
		last = sorted[1].y; // Include y1 scanline
	else 
		last = sorted[1].y - 1; // Skip it
	
	for (y = sorted[0].y; y <= last; y++) 
	{
		a = sorted[0].x + sa / dy01;
		b = sorted[0].x + sb / dy02;
		sa += dx01;
		sb += dx02;
		/* longhand:
		a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		
		if (a > b) swap16(a, b);
		raster_rgba_hline(pb, a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2. This loop is skipped if y1=y2.
	sa = dx12 * (y - sorted[1].y);
	sb = dx02 * (y - sorted[0].y);
	for (; y <= sorted[2].y; y++) 
	{
		a = sorted[1].x + sa / dy12;
		b = sorted[0].x + sb / dy02;
		sa += dx12;
		sb += dx02;
		/* longhand:
		a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
		b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
		*/
		if (a > b) 
			swap16(a, b);

		raster_rgba_hline(pb, a, y, b - a + 1, color);
	}
}
