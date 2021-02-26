/**
 * File: avif_im2im
 *
 * Sanity check for AVIF encoding and decoding.
 * We create a simple gd image, we encode it to AVIF, and we decode it back to gd.
 * Then we make sure the image we started with and the image we finish with are the same.
 *
 */

#include "gd.h"
#include "gdtest.h"
#include <stdio.h>

int main()
{
	gdImagePtr srcGdIm, destGdIm;
	void *avifImageDataPtr;
	FILE *fp;
	int r, g, b;
	int size = 0;
	CuTestImageResult result = {0, 0};

	// Create new gd image and add some shapes to it.
	srcGdIm = gdImageCreateTrueColor(100, 100);
	gdTestAssertMsg(srcGdIm != NULL, "could not create source image\n");

	r = gdImageColorAllocate(srcGdIm, 0xFF, 0, 0);
	g = gdImageColorAllocate(srcGdIm, 0, 0xFF, 0);
	b = gdImageColorAllocate(srcGdIm, 0, 0, 0xFF);
	gdImageFilledRectangle(srcGdIm, 0, 0, 99, 99, r);
	gdImageRectangle(srcGdIm, 20, 20, 79, 79, g);
	gdImageEllipse(srcGdIm, 70, 25, 30, 20, b);

	// Encode the gd image to a test AVIF file.
	fp = gdTestTempFp();
	gdImageAvif(srcGdIm, fp);
	fclose(fp);

	// Encode the gd image to an AVIF image in memory.
	avifImageDataPtr = gdImageAvifPtrEx(srcGdIm, &size, 100, 10);
	gdTestAssertMsg(avifImageDataPtr != NULL, "gdImageAvifPtr() returned null\n");
	gdTestAssertMsg(size > 0, "gdImageAvifPtr() returned a non-positive size\n");

	// Encode the AVIF image back into a gd image.
	destGdIm = gdImageCreateFromAvifPtr(size, avifImageDataPtr);
	gdTestAssertMsg(destGdIm != NULL, "gdImageAvifPtr() returned null\n");

	// Encode that gd image to a test AVIF file.
	fp = gdTestTempFp();
	gdImageAvif(destGdIm, fp);
	fclose(fp);

	// Make sure the image we started with is the same as the image after two conversions.
	 gdTestImageDiff(srcGdIm, destGdIm, NULL, &result);
	 gdTestAssertMsg(result.pixels_changed == 0, "pixels changed: %d\n", result.pixels_changed);

	if (srcGdIm)
		gdImageDestroy(srcGdIm);

	if (destGdIm)
		gdImageDestroy(destGdIm);

	if (avifImageDataPtr)
		gdFree(avifImageDataPtr);

	return gdNumFailures();
}
