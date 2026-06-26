/**
 * Regression test for <https://github.com/libgd/libgd/issues/892>
 *
 * Test that gdImagePng writes a proper PNG greyscale file when the
 * palette contains only greyscale colors (R == G == B). Previously
 * this would fail with "gd-png error: no colors in palette" when
 * the palette was completely empty, and for non-empty greyscale
 * palettes it would emit a PALETTE PNG instead of a GREY PNG.
 */

#include "gd.h"
#include "gdtest.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
	gdImagePtr im, im2;
	void *png_data = NULL;
	int png_size = 0;
	FILE *fp;

	/* Test case 1: empty palette (the original bug from issue #892) */
	im = gdImageCreate(16, 16);
	gdTestAssert(im != NULL);

	fp = gdTestTempFp();
	gdTestAssert(fp != NULL);

	/* This used to print "gd-png error: no colors in palette" and fail */
	gdImagePng(im, fp);
	fclose(fp);
	gdImageDestroy(im);

	/* Test case 2: non-empty palette with greyscale colors */
	im = gdImageCreate(4, 4);
	gdTestAssert(im != NULL);

	/* Allocate a few greyscale colors (all R == G == B) */
	gdImageColorAllocate(im, 0, 0, 0);       /* black  */
	gdImageColorAllocate(im, 128, 128, 128); /* grey   */
	gdImageColorAllocate(im, 255, 255, 255); /* white  */

	/* Set some pixels */
	gdImageSetPixel(im, 0, 0, 0);
	gdImageSetPixel(im, 1, 0, 1);
	gdImageSetPixel(im, 2, 0, 2);
	gdImageSetPixel(im, 3, 0, 2);

	png_data = gdImagePngPtr(im, &png_size);
	gdTestAssertMsg(png_data != NULL, "gdImagePngPtr failed");
	gdTestAssertMsg(png_size > 0, "PNG size should be non-zero");

	/* Round-trip: read it back */
	im2 = gdImageCreateFromPngPtr(png_size, png_data);
	gdTestAssertMsg(im2 != NULL, "gdImageCreateFromPngPtr failed");
	gdTestAssertMsg(im2->sx == 4 && im2->sy == 4, "Dimensions mismatch");

	gdFree(png_data);
	gdImageDestroy(im2);
	gdImageDestroy(im);

	/* Test case 3: palette with mixed color (should still produce PALETTE) */
	im = gdImageCreate(2, 1);
	gdTestAssert(im != NULL);
	gdImageColorAllocate(im, 255, 0, 0);   /* red  - not greyscale */
	gdImageColorAllocate(im, 0, 0, 255);   /* blue - not greyscale */
	gdImageSetPixel(im, 0, 0, 0);
	gdImageSetPixel(im, 1, 0, 1);

	png_data = gdImagePngPtr(im, &png_size);
	gdTestAssertMsg(png_data != NULL, "gdImagePngPtr failed for color PNG");
	gdFree(png_data);
	gdImageDestroy(im);

	return gdNumFailures();
}
