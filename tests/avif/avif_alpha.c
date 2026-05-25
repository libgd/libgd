/**
 * File: avif_alpha
 *
 * Test that AVIF encoding and decoding preserves the alpha channel correctly.
 * Loads dice_with_alpha.png, encodes it to AVIF, decodes the AVIF back,
 * and verifies that every pixel's alpha value matches the original PNG.
 */

#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main() {
	FILE *fp;
	gdImagePtr imFromPng = NULL, imFromAvif = NULL;
	void *avifImDataPtr = NULL;
	int size;
	int x, y, w, h;
	int failures = 0;

	fp = gdTestFileOpen2("avif", "dice_with_alpha.png");
	gdTestAssertMsg(fp != NULL, "Could not open dice_with_alpha.png\n");
	if (!fp)
		goto cleanup;

	imFromPng = gdImageCreateFromPng(fp);
	fclose(fp);
	fp = NULL;

	gdTestAssertMsg(imFromPng != NULL, "gdImageCreateFromPng failed for dice_with_alpha.png\n");
	if (!imFromPng)
		goto cleanup;

	avifImDataPtr = gdImageAvifPtrEx(imFromPng, &size, 100, 0);
	gdTestAssertMsg(avifImDataPtr != NULL, "gdImageAvifPtrEx failed\n");
	if (!avifImDataPtr)
		goto cleanup;

	imFromAvif = gdImageCreateFromAvifPtr(size, avifImDataPtr);
	gdTestAssertMsg(imFromAvif != NULL, "gdImageCreateFromAvifPtr failed\n");
	if (!imFromAvif)
		goto cleanup;

	w = gdImageSX(imFromPng);
	h = gdImageSY(imFromPng);

	gdTestAssertMsg(gdImageSX(imFromAvif) == w && gdImageSY(imFromAvif) == h,
	                "AVIF image dimensions differ from PNG\n");
	if (gdImageSX(imFromAvif) != w || gdImageSY(imFromAvif) != h)
		goto cleanup;

	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			int pngPixel  = gdImageGetPixel(imFromPng,  x, y);
			int avifPixel = gdImageGetPixel(imFromAvif, x, y);
			int pngAlpha  = gdTrueColorGetAlpha(pngPixel);
			int avifAlpha = gdTrueColorGetAlpha(avifPixel);

			if (pngAlpha != avifAlpha) {
				if (failures == 0) {
					/* Report only the first mismatch to keep output short */
					char msg[256];
					sprintf(msg,
					        "Alpha mismatch at (%d, %d): PNG alpha=%d, AVIF alpha=%d\n",
					        x, y, pngAlpha, avifAlpha);
					gdTestAssertMsg(0, msg);
				}
				failures++;
			}
		}
	}

	if (failures > 1) {
		char msg[256];
		sprintf(msg, "Total alpha mismatches: %d\n", failures);
		gdTestAssertMsg(0, msg);
	}

cleanup:
	if (imFromPng)    gdImageDestroy(imFromPng);
	if (imFromAvif)   gdImageDestroy(imFromAvif);
	if (avifImDataPtr) gdFree(avifImDataPtr);

	return gdNumFailures();
}
