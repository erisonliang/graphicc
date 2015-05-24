#include "drawproc.h"
#include "pipeline_opengl.h"

#include <stdio.h>
#include <math.h>

LRESULT CALLBACK keyPressed(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
//#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))


REAL winResx = 640;
REAL winResy = 480;
REAL winMaxX = winResx - 1;
REAL winMaxY = winResy - 1;

REAL winCenterx = (winResx / 2)-1;
REAL winCentery = (winResy / 2)-1;

real3 eye = { -0.25, 1, 2 };

LRESULT CALLBACK keyPressed(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case VK_RIGHT:
			eye[0] += 0.25;
		break;

		case VK_LEFT:
			eye[0] -= 0.25;
		break;

		case VK_UP:
			eye[1] += 0.125;
			break;

		case VK_DOWN:
			eye[1] -= 0.125;
			break;

		case VK_SPACE:
			write_PPM_binary("test_ogl_pipeline.ppm", gpb);
		break;
	}

	return 0;
}

LRESULT CALLBACK mouseWheel(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int16_t delta = HIWORD(wParam);
	if (delta > 0) {
		eye[2] -= 0.25;
	}
	else {
		eye[2] += 0.25;
	}

	return 0;
}

void setup()
{
	setOnKeyPressedHandler(keyPressed);
	setOnMouseWheelHandler(mouseWheel);

	size(winResx, winResy);
	background(pLightGray);
	//noLoop();
}

REAL pts1[] = {
	0, 0, 1, 1,
	2, 0, 1, 1,
	2, 3, 1, 1,
	0, 3, 1, 1,
	0, 0, 0, 1,
	2, 0, 0, 1,
	2, 3, 0, 1,
	0, 3, 0, 1
};

size_t nrows = sizeof(pts1) / sizeof(REAL) / 4;

/*
Reference: http://www.songho.ca/opengl/gl_transform.html
1. Your 4x4 PROJECTION transform takes you from 4D eye coordinates to 4D clip coordinates.
2. Then the perspective divide takes you from 4D clip coordinates to 3D NDC coordinates.
3. Then the viewport transformation takes those 3D NDC coordinates into 3D window coordinates.

*/
void transform_points(REAL *res, const mat4 &tmat, const REAL *inpts, const size_t nrows)
{
	for (size_t idx = 0; idx < nrows; idx++)
	{
		mat4_mul_col4(&res[idx * 4], tmat, &inpts[idx * 4]);
	}
}

void drawShape()
{
	// model to view
	mat4 mv_matrix;
	real3 center = { 0, 0, 0 };
	real3 up = { 0, 1, 0 };
	ogl_lookat(mv_matrix, eye, center, up);


	// projection
	REAL left = -5;
	REAL right = 5;
	REAL bottom = -5;
	REAL top = 5;
	REAL n = -100;
	REAL f = 100;

	mat4 proj_matrix = ogl_ortho(left, right, bottom, top, n, f);

	mat4 posMatrix;
	mat4_mul_mat4(posMatrix, proj_matrix, mv_matrix);

	// divide by w
	REAL res[32];
	transform_points(res, posMatrix, pts1, nrows);


	// viewport transform
	fill(pBlack);
	stroke(pBlack);
	REAL screenx, screeny;
	for (int idx = 0; idx < nrows; idx++)
	{
		REAL *pt = &res[idx * 4];
		ogl_map_to_window(screenx, screeny,
			pt[0], pt[1], 1,
			winMaxX, winMaxY,
			winCenterx, winCentery);
		printf("viewpoint: [%d, %d]\n", screenx, screeny);

		rectMode(CENTER);
		rect(screenx, screeny, 4, 4);
	}
}

void drawText()
{
	REAL screenx, screeny;
	REAL clipx = 0;
	REAL clipy = 0;
	REAL clipw = 1;

	// Draw lines
	//ogl_map_to_window(screenx, screeny, clipx, clipy, clipw,winMaxX, winMaxY,winCenterx, winCentery);

	// draw text at the center
	ogl_map_to_window(screenx, screeny,
		clipx, clipy, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);

	//printf("screenx: %d  screeny: %d\n", screenx, screeny);
	fill(pWhite);
	rectMode(CENTER);
	rect(screenx, screeny, 20, 20);

	// Left edge
	fill(pBlack);
	// Lefts
	ogl_map_to_window(screenx, screeny,
		-1, 1, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_LEFT, TX_TOP);
	text("LEFT, TOP (-1, 1)", screenx, screeny);

	ogl_map_to_window(screenx, screeny,
		-1, 0, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_LEFT, TX_CENTER);
	text("LEFT, CENTER (-1,0)", screenx, screeny);

	ogl_map_to_window(screenx, screeny,
		-1, -1, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_LEFT, TX_BOTTOM);
	text("LEFT, BOTTOM (-1, -1)", screenx, screeny);


	// Centers
	ogl_map_to_window(screenx, screeny,
		0, 1, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_CENTER, TX_TOP);
	text("CENTER, TOP (0, 1)", screenx, screeny);


	ogl_map_to_window(screenx, screeny,
		0, 0, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_CENTER, TX_CENTER);
	text("CENTER, CENTER (0, 0)", screenx, screeny);

	ogl_map_to_window(screenx, screeny,
		0, -1, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_CENTER, TX_BOTTOM);
	text("CENTER, BOTTOM (0, 0)", screenx, screeny);

	// Rights
	ogl_map_to_window(screenx, screeny,
		1, 1, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_RIGHT, TX_TOP);
	text("RIGHT, TOP (1, 1)", screenx, screeny);

	ogl_map_to_window(screenx, screeny,
		1, 0, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_RIGHT, TX_CENTER);
	text("RIGHT, CENTER (1,0)", screenx, screeny);

	ogl_map_to_window(screenx, screeny,
		1, -1, clipw,
		winMaxX, winMaxY,
		winCenterx, winCentery);
	textAlign(TX_RIGHT, TX_BOTTOM);
	text("RIGHT, BOTTOM (1, -1)", screenx, screeny);

}

void drawAxes()
{
	REAL axispts1[] = {
		0, 0, 0, 1,
		10, 0, 0, 1,
		0, 10, 0, 1,
		0, 0, 10, 1,
	};

	size_t nrows = sizeof(axispts1) / sizeof(REAL) / 4;

	// model to view
	mat4 mv_matrix;
	real3 center = { 0, 0, 0 };
	real3 up = { 0, 1, 0 };
	ogl_lookat(mv_matrix, eye, center, up);


	// projection
	REAL left = -5;
	REAL right = 5;
	REAL bottom = -5;
	REAL top = 5;
	REAL n = -100;
	REAL f = 100;

	mat4 proj_matrix = ogl_ortho(left, right, bottom, top, n, f);

	mat4 posMatrix;
	mat4_mul_mat4(posMatrix, proj_matrix, mv_matrix);

	// divide by w
	REAL res[32];
	transform_points(res, posMatrix, axispts1, nrows);


	// viewport transform
	REAL originx, originy;
	REAL screenx, screeny;
	REAL *originpt = &res[0];
	REAL *xaxisend = &res[1 * 4];
	REAL *yaxisend = &res[2 * 4];
	REAL *zaxisend = &res[3 * 4];

	ogl_map_to_window(originx, originy,
			originpt[0], originpt[1], 1,
			winMaxX, winMaxY,
			winCenterx, winCentery);

	ogl_map_to_window(screenx, screeny,
		xaxisend[0], xaxisend[1], 1,
		winMaxX, winMaxY,
		winCenterx, winCentery);

	stroke(pRed);
	line(originx, originy, screenx, screeny);

	// Y-axis
	ogl_map_to_window(screenx, screeny,
		yaxisend[0], yaxisend[1], 1,
		winMaxX, winMaxY,
		winCenterx, winCentery);

	stroke(pGreen);
	line(originx, originy, screenx, screeny);

	// Z-axis
	ogl_map_to_window(screenx, screeny,
		zaxisend[0], zaxisend[1], 1,
		winMaxX, winMaxY,
		winCenterx, winCentery);

	stroke(pBlue);
	line(originx, originy, screenx, screeny);

}

void draw()
{
	background(cornsilk);
	
	//	drawText();
	drawAxes();
	drawShape();

}