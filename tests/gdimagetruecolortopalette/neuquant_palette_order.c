#include "gd.h"
#include "gdtest.h"

static gdImagePtr create_gray_bands(void)
{
	gdImagePtr im;
	int x, y;

	im = gdImageCreateTrueColor(32, 32);
	if (im == NULL) {
		return NULL;
	}

	for (y = 0; y < gdImageSY(im); y++) {
		for (x = 0; x < gdImageSX(im); x++) {
			int gray = (x < 8) ? 0 : (x < 16) ? 85 : (x < 24) ? 170 : 255;
			gdImageSetPixel(im, x, y, gdTrueColor(gray, gray, gray));
		}
	}

	return im;
}

int main(void)
{
	gdImagePtr im;
	gdImagePtr pal;
	int i;

	im = create_gray_bands();
	if (!gdTestAssert(im != NULL)) {
		return gdNumFailures();
	}

	pal = gdImageNeuQuant(im, 256, 1);
	if (!gdTestAssert(pal != NULL)) {
		gdImageDestroy(im);
		return gdNumFailures();
	}

	gdTestAssert(!gdImageTrueColor(pal));
	gdTestAssert(gdImageSX(pal) == gdImageSX(im));
	gdTestAssert(gdImageSY(pal) == gdImageSY(im));
	gdTestAssert(pal->colorsTotal == 256);

	for (i = 1; i < pal->colorsTotal; i++) {
		gdTestAssertMsg(pal->green[i - 1] <= pal->green[i],
			"palette green channel must remain ordered at entries %d and %d (%d > %d)",
			i - 1, i, pal->green[i - 1], pal->green[i]);
	}

	for (i = 1; i < gdImageSX(pal); i++) {
		int prev = gdImageGetPixel(pal, i - 1, 0);
		int current = gdImageGetPixel(pal, i, 0);
		gdTestAssertMsg(prev <= current,
			"pixel indexes must remain ordered at x=%d and x=%d (%d > %d)",
			i - 1, i, prev, current);
	}

	gdImageDestroy(pal);
	gdImageDestroy(im);

	return gdNumFailures();
}
