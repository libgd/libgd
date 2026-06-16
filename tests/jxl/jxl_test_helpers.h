#ifndef JXL_TEST_HELPERS_H
#define JXL_TEST_HELPERS_H

#include "gd.h"
#include "gdtest.h"

static inline gdImagePtr jxlTestCreatePattern(int alpha) {
	gdImagePtr im;
	int red, green, blue, semi;

	im = gdImageCreateTrueColor(32, 24);
	if (im == NULL) {
		return NULL;
	}

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, alpha);

	red = gdTrueColorAlpha(255, 0, 0, gdAlphaOpaque);
	green = gdTrueColorAlpha(0, 255, 0, gdAlphaOpaque);
	blue = gdTrueColorAlpha(0, 0, 255, gdAlphaOpaque);
	semi = gdTrueColorAlpha(255, 255, 0, 63);

	gdImageFilledRectangle(im, 0, 0, 31, 23, red);
	gdImageFilledRectangle(im, 4, 4, 27, 19, green);
	gdImageEllipse(im, 16, 12, 18, 10, blue);
	if (alpha) {
		gdImageFilledRectangle(im, 10, 6, 21, 17, semi);
	}

	return im;
}

static inline gdImagePtr jxlTestCreateSolid(int r, int g, int b, int a) {
	gdImagePtr im;
	int color;
	int x, y;

	im = gdImageCreateTrueColor(4, 4);
	if (im == NULL) {
		return NULL;
	}

	gdImageAlphaBlending(im, 0);
	gdImageSaveAlpha(im, 1);
	color = gdTrueColorAlpha(r, g, b, a);

	for (y = 0; y < 4; y++) {
		for (x = 0; x < 4; x++) {
			gdImageSetPixel(im, x, y, color);
		}
	}

	return im;
}

static inline void *jxlTestEncodeAnimation(gdImagePtr first, int first_delay,
										   gdImagePtr second, int second_delay,
										   int lossless, int *size) {
	gdJxlAnimPtr anim;
	void *data = NULL;

	*size = 0;
	anim = gdImageJxlAnimBeginPtr(gdImageSX(first), gdImageSY(first),
								  lossless, lossless ? 0.0f : 1.0f, 7);
	if (anim == NULL) {
		return NULL;
	}

	if (!gdImageJxlAnimAddFrame(anim, first, first_delay) ||
		!gdImageJxlAnimAddFrame(anim, second, second_delay)) {
		gdImageJxlAnimEndPtr(anim, size);
		return NULL;
	}

	data = gdImageJxlAnimEndPtr(anim, size);
	return data;
}

#endif
