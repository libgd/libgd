#include <gd.h>
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;
	int c, i;

	im = gdImageCreateTrueColor(5, 5);
	c = gdImageColorClosest(im, 255, 0, 255);
	gdImageDestroy(im);

	if (gdTestAssert(c==0xFF00FF) != 1) {
		error = -1;
	}

	im = gdImageCreate(5, 5);
	c = gdImageColorClosest(im, 255, 0, 255);
	if (gdTestAssert(c==-1) != 1) {
		error = -1;
	}
	gdImageDestroy(im);

	im = gdImageCreate(5, 5);
	c = gdImageColorAllocate(im, 255, 0, 255);
	c = gdImageColorClosest(im, 255, 0, 255);
	c = gdTrueColorAlpha(gdImageRed(im, c), gdImageGreen(im, c), gdImageBlue(im, c), 0);
	gdImageDestroy(im);
	if (gdTestAssert(c==0xFF00FF) != 1) {
		error = -1;
	}


	im = gdImageCreate(5, 5);
	for (i=0; i < 255; i++) {
		c = gdImageColorAllocate(im, 255, 0, 0);
	}
	c = gdImageColorClosest(im, 255, 0, 0);
	c = gdTrueColorAlpha(gdImageRed(im, c), gdImageGreen(im, c), gdImageBlue(im, c), 0);
	gdImageDestroy(im);
	if (gdTestAssert(c==0xFF0000) != 1) {
		error = -1;
	}

	im = gdImageCreate(5, 5);
	for (i=0; i < 256; i++) {
		c = gdImageColorAllocate(im, 255, 0, 0);
	}
	c = gdImageColorClosest(im, 255, 0, 0);
	c = gdTrueColorAlpha(gdImageRed(im, c), gdImageGreen(im, c), gdImageBlue(im, c), 0);
	gdImageDestroy(im);
	if (gdTestAssert(c==0xFF0000) != 1) {
		error = -1;
	}

	return error;
}
