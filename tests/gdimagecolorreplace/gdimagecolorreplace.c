#include <gd.h>
#include <stdio.h>
#include <limits.h>
#include "gdtest.h"

static int callback(gdImagePtr im, int src)
{
	int r, g, b;

	r = gdImageRed(im, src);
	g = gdImageGreen(im, src);
	b = gdImageBlue(im, src);
	if (b & 0xFF) {
		return gdImageColorResolve(im, 0x0F & r, 0x0F & g, 0);
	} else {
		return -1;
	}
}

static void run_tests(gdImagePtr im, int *error)
{
	int white, black, c, d;
	int src[2], dst[2];
	int n;

#define CHECK_VALUE(n, expected) do {							\
		if (gdTestAssert((n) == (expected)) != 1) {				\
			printf("%d is expected, but %d\n", expected, n);	\
			*error = -1;										\
		}														\
	} while (0)
#define CHECK_PIXEL(x, y, expected) do {						\
		gdImageSetClip(im, 0, 0, 4, 4);							\
		c = gdImageGetPixel(im, (x), (y));						\
		if (gdTestAssert(c == (expected)) != 1) {				\
			printf("%d is expected, but %d\n", expected, c);	\
			*error = -1;										\
		}														\
	} while (0)

	white = gdImageColorAllocate(im, 0xFF, 0xFF, 0xFF);
	black = gdImageColorAllocate(im, 0, 0, 0);
	c = gdImageColorAllocate(im, 0xFF, 0, 0xFF);
	gdImageFilledRectangle(im, 0, 0, 4, 4, white);
	gdImageFilledRectangle(im, 0, 0, 3, 3, black);
	n = gdImageColorReplace(im, white, c);
	CHECK_VALUE(n, 9);
	CHECK_PIXEL(0, 0, black);
	CHECK_PIXEL(2, 3, black);
	CHECK_PIXEL(4, 4, c);

	gdImageSetClip(im, 1, 1, 3, 3);
	n = gdImageColorReplace(im, black, c);
	CHECK_VALUE(n, 9);
	CHECK_PIXEL(0, 0, black);
	CHECK_PIXEL(2, 3, c);

	src[0] = black;
	src[1] = c;
	dst[0] = c;
	dst[1] = white;
	gdImageSetClip(im, 0, 0, 4, 4);
	n = gdImageColorReplaceArray(im, 2, src, dst);
	CHECK_VALUE(n, 25);
	CHECK_PIXEL(0, 0, c);
	CHECK_PIXEL(2, 3, white);
	CHECK_PIXEL(4, 4, white);

	n = gdImageColorReplaceArray(im, 0, src, dst);
	CHECK_VALUE(n, 0);
	n = gdImageColorReplaceArray(im, -1, src, dst);
	CHECK_VALUE(n, 0);
	n = gdImageColorReplaceArray(im, INT_MAX, src, dst);
	CHECK_VALUE(n, -1);

	gdImageSetClip(im, 1, 1, 4, 4);
	n = gdImageColorReplaceCallback(im, callback);
	CHECK_VALUE(n, 16);
	CHECK_PIXEL(0, 0, c);
	CHECK_PIXEL(0, 4, white);
	d = gdImageColorExact(im, 0x0F, 0x0F, 0);
	if (gdTestAssert(d > 0) != 1) {
		*error = -1;
	}
	CHECK_PIXEL(2, 3, d);
	CHECK_PIXEL(4, 4, d);

#undef CHECK_VALUE
#undef CHECK_PIXEL
}

int main()
{
	gdImagePtr im;
	int error = 0;

	/* true color */
	im = gdImageCreateTrueColor(5, 5);
	run_tests(im, &error);
	gdImageDestroy(im);
	
	/* palette */
	im = gdImageCreate(5, 5);
	run_tests(im, &error);
	gdImageDestroy(im);

	return error;
}
