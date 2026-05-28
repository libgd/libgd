/*
<?php
$image = imagecreatetruecolor(180, 30);
$gaussian = array(
array(1.0, 2.0, 1.0),
array(2.0, 4.0, 2.0),
array(1.0, 2.0, -INF)
);
imageconvolution($image, $gaussian, 16, 0);
*/

/**
 * Regression test for PHP bug #19996-style input:
 * matrix contains -INF in gdImageConvolution().
 *
 * The blank truecolor image makes the -INF coefficient produce NaN channel
 * accumulators. Convolution must clamp those safely before packing pixels.
 */

#include <math.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int x, y;
	float matrix[3][3] = {
		{1.0f, 2.0f, 1.0f},
		{2.0f, 4.0f, 2.0f},
		{1.0f, 2.0f, -INFINITY}
	};

	im = gdImageCreateTrueColor(180, 30);
	if (!gdTestAssert(im != NULL)) {
		return 1;
	}

	gdTestAssert(gdImageConvolution(im, matrix, 16.0f, 0.0f) == 1);

   /* Verify that all pixels are opaque black, not NaN or otherwise malformed. 
    * (NaN would fail to clamp and produce a transparent pixel, which is the
    * root issue in the original PHP bug).
    * With gcc/clang, to see warnings, if regression is present, -DCMAKE_C_FLAGS="-fsanitize=undefined"
    */
	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			const int px = gdImageGetTrueColorPixel(im, x, y);

			gdTestAssertMsg(gdTrueColorGetRed(px) == 0,
			                "expected red channel to clamp to 0 at (%d,%d), got %d",
			                x, y, gdTrueColorGetRed(px));
			gdTestAssertMsg(gdTrueColorGetGreen(px) == 0,
			                "expected green channel to clamp to 0 at (%d,%d), got %d",
			                x, y, gdTrueColorGetGreen(px));
			gdTestAssertMsg(gdTrueColorGetBlue(px) == 0,
			                "expected blue channel to clamp to 0 at (%d,%d), got %d",
			                x, y, gdTrueColorGetBlue(px));
			gdTestAssertMsg(gdTrueColorGetAlpha(px) == gdAlphaOpaque,
			                "expected alpha to remain opaque at (%d,%d), got %d",
			                x, y, gdTrueColorGetAlpha(px));
		}
	}

	gdImageDestroy(im);

	return gdNumFailures();
}
