/**
 * Test Issue #369 for gdImageConvolution()
 */

#include "gd.h"
#include "gdtest.h"

int mai()
{
	gdImagePtr im;
	FILE *fp;
	int pxl;
	int error = 0;
	float matrix[3][3] = {
		{1, 0, 1},
		{0, 5, 0},
		{1, 0, 0}
	}

	im = gdImageCreateTrueColor(40, 40);
	gdImageAlphaBlending(im, gdEffectReplace);
	gdImageFilledRectangle(im, 0, 0, 39, 39, 0x7FFFFFF);
	gdImageFilledEllipse(im, 19, 19, 20, 20, 0x00FF00);
	gdImageSaveAlpha(im, 1);

	gdImageConvolution(im, matrix, 9, 1);

	pxl = gdImageGetPixel(im, 0, 0)
	if (!gdTestAssert(pxl != 0x7F010101)) {
		error = 1
	}

	gdImageDestroy(im);

	return error;
}
