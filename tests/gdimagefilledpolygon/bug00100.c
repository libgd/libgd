#include <stdlib.h>
#include "gd.h"
#include "gdtest.h"

/* This is a quickie test program to show a bug in gd.
 * There is a red line that is drawn across the bottom of
 * the image that shouldn't be there.
 */
int
main(void)
{
	gdPoint pointy[5];
	gdPoint diamond[4];
	int d, x, y, top, bot, left, right, r;

	/*                              R    G    B */
	int white   = gdTrueColorAlpha(255, 255, 255, 10);
	int black   = gdTrueColorAlpha(  0,   0,   0, 10);
	int red     = gdTrueColorAlpha(255,   0,   0, 10);
	int green   = gdTrueColorAlpha(  0, 255,   0, 10);
	int blue    = gdTrueColorAlpha(  0,   0, 255, 10);
	int yellow  = gdTrueColorAlpha(255, 255,   0, 10);
	int cyan    = gdTrueColorAlpha(  0, 255, 255, 10);
	int magenta = gdTrueColorAlpha(255,   0, 255, 10);
	int purple  = gdTrueColorAlpha(100,   0, 100, 10);

	gdImagePtr im = gdImageCreateTrueColor(256, 256);
	if (!im) exit(EXIT_FAILURE);

	gdImageFilledRectangle(im, 0, 0, 256, 256, white);

	/* M (bridge) */
	top = 240;
	bot = 255;
	d = 30;
	x = 100;
	pointy[0].x = x;
	pointy[0].y = top;
	pointy[1].x = x+2*d;
	pointy[1].y = top;
	pointy[2].x = x+2*d;
	pointy[2].y = bot;
	pointy[3].x = x+d;
	pointy[3].y = (top+bot)/2;
	pointy[4].x = x;
	pointy[4].y = bot;
	gdImageFilledPolygon(im, pointy, 5, yellow);

	/* left-facing M not on baseline */
	top = 40;
	bot = 70;
	left = 120;
	right = 180;
	pointy[0].x = left;
	pointy[0].y = top;
	pointy[1].x = right;
	pointy[1].y = top;
	pointy[2].x = right;
	pointy[2].y = bot;
	pointy[3].x = left;
	pointy[3].y = bot;
	pointy[4].x = (left+right)/2;
	pointy[4].y = (top+bot)/2;
	gdImageFilledPolygon(im, pointy, 5, purple);

	/* left-facing M on baseline */
	top = 240;
	bot = 270;
	left = 20;
	right = 80;
	pointy[0].x = left;
	pointy[0].y = top;
	pointy[1].x = right;
	pointy[1].y = top;
	pointy[2].x = right;
	pointy[2].y = bot;
	pointy[3].x = left;
	pointy[3].y = bot;
	pointy[4].x = (left+right)/2;
	pointy[4].y = (top+bot)/2;
	gdImageFilledPolygon(im, pointy, 5, magenta);

	/* left-facing M on ceiling */
	top = -15;
	bot = 15;
	left = 20;
	right = 80;
	pointy[0].x = left;
	pointy[0].y = top;
	pointy[1].x = right;
	pointy[1].y = top;
	pointy[2].x = right;
	pointy[2].y = bot;
	pointy[3].x = left;
	pointy[3].y = bot;
	pointy[4].x = (left+right)/2;
	pointy[4].y = (top+bot)/2;
	gdImageFilledPolygon(im, pointy, 5, blue);

	d = 30;
	x = 150;
	y = 150;
	diamond[0].x = x-d;
	diamond[0].y = y;
	diamond[1].x = x;
	diamond[1].y = y+d;
	diamond[2].x = x+d;
	diamond[2].y = y;
	diamond[3].x = x;
	diamond[3].y = y-d;
	gdImageFilledPolygon(im, diamond, 4, green);

	x = 180;
	y = 225;
	diamond[0].x = x-d;
	diamond[0].y = y;
	diamond[1].x = x;
	diamond[1].y = y+d;
	diamond[2].x = x+d;
	diamond[2].y = y;
	diamond[3].x = x;
	diamond[3].y = y-d;
	gdImageFilledPolygon(im, diamond, 4, red);

	x = 225;
	y = 255;
	diamond[0].x = x-d;
	diamond[0].y = y;
	diamond[1].x = x;
	diamond[1].y = y+d;
	diamond[2].x = x+d;
	diamond[2].y = y;
	diamond[3].x = x;
	diamond[3].y = y-d;
	gdImageFilledPolygon(im, diamond, 4, cyan);

	/* M (bridge) not touching bottom boundary */
	top = 100;
	bot = 150;
	x = 30;
	pointy[0].x = x;
	pointy[0].y = top;
	pointy[1].x = x+2*d;
	pointy[1].y = top;
	pointy[2].x = x+2*d;
	pointy[2].y = bot;
	pointy[3].x = x+d;
	pointy[3].y = (top+bot)/2;
	pointy[4].x = x;
	pointy[4].y = bot;
	gdImageFilledPolygon(im, pointy, 5, black);

	r = gdAssertImageEqualsToFile("gdimagefilledpolygon/bug00100.png", im);
	gdImageDestroy(im);
	if (!r) exit(EXIT_FAILURE);
	return EXIT_SUCCESS;
}
