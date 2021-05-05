/**
 * Regression test for <https://github.com/libgd/libgd/issues/329>
 *
 * We're testing that for truecolor as well as palette images after
 * GD_BILINEAR_FIXED scaling the corner pixels of the scaled image have the
 * expected color.
 */

#include <string.h>
#include "gd.h"
#include "gdtest.h"

static void test(const char *mode)
{
	gdImagePtr src, dst;
	int expected, actual;

	if (strcmp(mode, "palette")) {
		src = gdImageCreateTrueColor(100, 100);
		expected = gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque);
		gdImageFilledRectangle(src, 0,0, 99,99, expected);
	} else {
		src = gdImageCreate(100, 100);
		gdImageColorAllocate(src, 255, 255, 255);
		expected = gdImageGetTrueColorPixel(src, 49, 49);
	}

	gdImageSetInterpolationMethod(src, GD_BILINEAR_FIXED);
	dst = gdImageScale(src, 200, 200);

	actual = gdImageGetPixel(dst, 0, 0);
	gdTestAssertMsg(actual == expected, "%s: wrong color; expected %x, but got %x", mode, expected, actual);
	actual = gdImageGetPixel(dst, 0, 199);
	gdTestAssertMsg(actual == expected, "%s: wrong color; expected %x, but got %x", mode, expected, actual);
	actual = gdImageGetPixel(dst, 199, 199);
	gdTestAssertMsg(actual == expected, "%s: wrong color; expected %x, but got %x", mode, expected, actual);
	actual = gdImageGetPixel(dst, 199, 0);
	gdTestAssertMsg(actual == expected, "%s: wrong color; expected %x, but got %x", mode, expected, actual);

	gdImageDestroy(src);
	gdImageDestroy(dst);
}

int main()
{
	test("palette");
	test("truecolor");

	return gdNumFailures();
}
