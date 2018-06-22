/**
 * Regression test for <https://github.com/libgd/libgd/issues/330>.
 *
 * We're testing that after scaling a palette image, the center pixel actually
 * has the expected color value.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	int color;

	src = gdImageCreate(100, 100);
	gdImageColorAllocate(src, 255, 255, 255);

	gdImageSetInterpolationMethod(src, GD_BILINEAR_FIXED);
	dst = gdImageScale(src, 200, 200);

	color = gdImageGetPixel(dst, 99, 99);
	gdTestAssertMsg(color == 0xffffff,
	                "expected color ffffff, but got %x\n", color);

	gdImageDestroy(src);
	gdImageDestroy(dst);

	return 0;
}
