#include "drawproc.h"

int drawMode = 0;
static const int maxModes = 3;

void keyReleased()
{
	if (keyCode == VK_SPACE) {
		drawMode += 1;
		if (drawMode >= maxModes) {
			drawMode = 0;
		}
	}

	draw();
}

void setup()
{
	size(200, 200);

	//noLoop();
}

void drawRGB()
{
	noStroke();
	colorMode(COLOR_MODE_RGB, width);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			stroke(i, j, 0);
			point(i, j);
		}
	}
}

void drawHSB()
{
	noStroke();
	colorMode(COLOR_MODE_HSB, width);
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			stroke(i, j, width);
			point(i, j);
		}
	}
}

void drawTeal()
{
	COLOR bg;  

	colorMode(COLOR_MODE_HSB, 360, 100, 100);
	bg = color(180, 50, 50);

	backgroundRGBA(bg);
}

void draw()
{
	background(0);

	switch (drawMode) {
		case 0:
			drawRGB();
		break;

		case 1:
			drawHSB();
		break;

		case 2:
			drawTeal();
		break;
	}

}