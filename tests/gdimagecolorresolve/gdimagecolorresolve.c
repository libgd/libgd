#include <gd.h>
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;
	int c, c1, c2, c3, c4, color, i;

	im = gdImageCreateTrueColor(5, 5);
	c = gdImageColorResolve(im, 255, 0, 255);
	c2 = gdImageColorResolveAlpha(im, 255, 0, 255, 100);
	gdImageDestroy(im);

	if (gdTestAssert(c == 0xFF00FF) != 1) {
		error = -1;
	}
	if (gdTestAssert(c2 == 0x64FF00FF) != 1) {
		error = -1;
	}

	im = gdImageCreate(5, 5);

	c1 = gdImageColorResolve(im, 255, 0, 255);
	c2 = gdImageColorResolve(im, 255, 200, 0);
	c3 = gdImageColorResolveAlpha(im, 255, 0, 255, 100);
	c4 = gdImageColorResolveAlpha(im, 255, 34, 255, 100);

	if (gdTestAssert(c1 == 0) != 1) {
		error = -1;
	}
	if (gdTestAssert(c2 == 1) != 1) {
		error = -1;
	}
	if (gdTestAssert(c3 == 2) != 1) {
		error = -1;
	}
	if (gdTestAssert(c4 == 3) != 1) {
		error = -1;
	}

	color = gdTrueColorAlpha(gdImageRed(im, c1), gdImageGreen(im, c1),
					gdImageBlue(im, c1), 0);
	if (gdTestAssert(color == 0xFF00FF) != 1) {
		error = -1;
	}
	color = gdTrueColorAlpha(gdImageRed(im, c2), gdImageGreen(im, c2),
					gdImageBlue(im, c2), 0);
	if (gdTestAssert(color == 0xFFC800) != 1) {
		error = -1;
	}
	color = gdTrueColorAlpha(gdImageRed(im, c3), gdImageGreen(im, c3),
					gdImageBlue(im, c3), 0);
	if (gdTestAssert(color == 0xFF00FF) != 1) {
		error = -1;
	}
	color = gdTrueColorAlpha(gdImageRed(im, c4), gdImageGreen(im, c4),
					gdImageBlue(im, c4), 0);
	if (gdTestAssert(color == 0xFF22FF) != 1) {
		error = -1;
	}
	gdImageDestroy(im);

	return error;
}
