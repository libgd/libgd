/**
 * Test Issue #369 for gdImageConvolution()
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	float matrix[3][3] = {
		{1, 0, 1},
		{0, 5, 0},
		{1, 0, 0}
	};

	im = gdImageCreateTrueColor(40, 40);
	gdImageAlphaBlending(im, gdEffectReplace);
	gdImageFilledRectangle(im, 0, 0, 39, 39, 0x7FFFFFFF);
	gdImageFilledEllipse(im, 19, 19, 20, 20, 0x00FF00);

	gdImageConvolution(im, matrix, 9, 1);
	gdTestAssert(0x7F010101 == gdImageGetPixel(im, 0, 0));

	gdImageDestroy(im);

	return gdNumFailures();
}
