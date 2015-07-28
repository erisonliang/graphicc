#pragma once

#define BGR_DOMINANT 1

#include "animwin32.h"

#include "graphicc.h"
#include "linearalgebra.h"
#include "raster_rgba.h"

#include "agg_embedded_raster_fonts.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef void (*EventObserverHandler)();


// IO Event Handlers
DPROC_API void keyPressed();
DPROC_API void keyReleased();
DPROC_API void keyTyped();

DPROC_API void mouseClicked();
DPROC_API void mouseDragged();
DPROC_API void mouseMoved(); 
DPROC_API void mousePressed();
DPROC_API void mouseReleased();

#ifdef __cplusplus
}
#endif

void redraw();

/*
Drawing API
*/
typedef uint32_t COLOR;

typedef enum COLORMODE {
	COLOR_MODE_RGB,
	COLOR_MODE_HSB
};
typedef enum RECTMODE{
	CORNER,
	CORNERS,
	RADIUS,
	CENTER
};

typedef enum ALIGNMENT {
	TX_CENTER	= 0x00,
	TX_LEFT		= 0x01,
	TX_RIGHT	= 0x04,
	TX_TOP		= 0x10,
	TX_BOTTOM	= 0x40

};
typedef enum GEOMETRY {
	GR_POINTS,
	GR_LINES,
	GR_LINE_STRIP,
	GR_LINE_LOOP,
	GR_POLYGON,
	GR_QUADS,
	GR_QUAD_STRIP,
	GR_TRIANGLES,
	GR_TRIANGLE_STRIP,
	GR_TRIANGLE_FAN
};

typedef enum KindOfClose {
	STROKE,
	CLOSE
};

typedef float coord;

struct Vector2d
{
	Vector2d(coord x, coord y)
	:_x(x), _y(y)
	{};

	coord _x;
	coord _y;
};

struct Vector3d
{
	union {
		struct {
			coord _x, _y, _z;
		};
		coord _v[3];
	};
};

// Global variables
extern size_t width;
extern size_t height;

extern pb_rgba *gpb;
extern pb_rect pixelFrame;


extern uint32_t bgColor;
extern pb_rgba *bgImage;

extern RECTMODE grectMode;
extern int gellipseMode;

// Initialization
void init();
void noLoop();
void loop();

// time
uint64_t millis();

// Drawing API
void size(const size_t width, const size_t height);



// Mouse
// Mouse
extern int mouseX;
extern int mouseY;
extern bool isMousePressed;
// pmouseX
// pmouseY
// mouseButton

// mouseClicked()
// mouseDragged()
// mouseMoved()
// mousePressed()
// mouseReleased()
// mouseWheel()



// Keyboard
extern int key;
extern int keyCode;
extern bool isKeyPressed;
// keyPressed()
// keyReleased()
// keyTyped()

// Setting colors
void colorMode(const COLORMODE mode, const float max1=-1, const float max2=-1, const float max3=-1, const float maxA=-1);
COLOR color(const float v1, const float v2=-1, const float v3=-1, const float alpha=-1);
void background(const float v1, const float v2 = -1, const float v3 = -1, const float alpha = -1);
void backgroundRGBA(const COLOR value);
void backgroundImage(pb_rgba *bg);
//void clear();

//void colorMode();
void noFill();
void fill(const float v1, const float v2 = -1, const float v3 = -1, const float alpha = -1);
void fillRGBA(const COLOR value);

void noStroke();
void strokeRGBA(const COLOR value);
void stroke(const float v1, const float v2 = -1, const float v3 = -1, const float alpha=-1);

// attributes
void ellipseMode(const int mode);
void noSmooth();
void rectMode(const RECTMODE mode);
//void smooth();
//void strokeCap();
//void strokeJoin();
void strokeWeight(const float weight);


// 2D primitives
void bezier(const int x1, const int y1, const int x2, const int y2, const int x3, const int y3, const int segments=60);
void ellipse(const int a, const int b, const int c, const int d);
void line(const int x1, const int y1, const int x2, const int y2);
void lineloop(const size_t nPtr, const int *pts);
void point(const int x, const int y);
void rect(const int a, const int b, const int c, const int d);
void quad(const int x1, const int y1, const int x2, const int y2, const int x3, const int y3, const int x4, const int y4);
void triangle(const int x1, const int y1, const int x2, const int y2, const int x3, const int y3);
void polygon(int nverts, int *a);

// Text
extern font_t gfont;

// createFont
// loadFont()
void setFont(const uint8_t *fontdata);
void text(const char *str, const int x, const int y);
//void textFont(font_t *font);
void textAlign(const int alignX=TX_LEFT, const int alignY = TX_BOTTOM);
// textLeading()
// textMode()
void textSize(const int size);
// textWidth()
// textAscent()
// textDescent()

// Shape
void beginShape(const int shapeKind = GR_POLYGON);
void vertex(const int x, const int y);
void bezierVertex(const int x1, const int y1, const int x2, const int y2, const int x3, const int y3);
void endShape(const int kindOfClose = STROKE);

// Math
int random(const int);

// Some color manipulation routines
inline uint32_t darker(const uint32_t acolor, const float factor = 0.60)
{
	uint8_t red = GET_R(acolor) *factor;
	uint8_t green = GET_G(acolor) * factor;
	uint8_t blue = GET_B(acolor) * factor;
	uint8_t alpha = GET_A(acolor);

	return RGBA(red, green, blue, alpha);
}

inline uint32_t brighter(const uint32_t acolor, const float factor=0.80)
{
	uint8_t red = (min(GET_R(acolor) *(1.0 / factor), 255));
	uint8_t green = (min(GET_G(acolor) * (1.0 / 0.85), 255));
	uint8_t blue = (min(GET_B(acolor) * (1.0 / factor), 255));
	uint8_t alpha = GET_A(acolor);

	return RGBA(red, green, blue, alpha);
}


#define pRed		RGBA(255, 0, 0, 255)
#define pGreen		RGBA(0, 255, 0, 255)
#define pBlue		RGBA(0, 0, 255, 255)

#define pBlack		RGBA(0, 0, 0, 255)
#define pWhite		RGBA(255, 255, 255, 255)
#define pYellow		RGBA(255, 255, 0, 255)
#define pTurquoise	RGBA(0, 255, 255, 255)
#define pDarkGray	RGBA(93, 93, 93, 255)
#define pGray		RGBA(127,127,127,255)
#define pLightGray	RGBA(163, 163, 163, 255)

#define aliceblue	RGBA(240, 248, 255, 255)
#define cornsilk	RGBA(255, 248, 220, 255)

