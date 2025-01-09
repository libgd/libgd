/*
 * Test that gdImageRotateInterpolated() never converts the src to palette.
 */

#include "gd.h"
#include "gdtest.h"

void rotate(int method, float angle)
{
    gdImagePtr src, dst;
    int black;

    src = gdImageCreate(10, 10);
    black = gdImageColorAllocate(src, 0, 0, 0);

    gdTestAssert(gdImageTrueColor(src) == 0);
    gdImageSetInterpolationMethod(src, method);
    dst = gdImageRotateInterpolated(src, angle, black);
    gdTestAssert(dst != NULL);
    gdTestAssert(gdImageTrueColor(src) == 0);
    gdTestAssert(dst != src);

	gdImageDestroy(src);
	gdImageDestroy(dst);
}

int main()
{
    rotate(GD_DEFAULT, 30.0);
    rotate(GD_NEAREST_NEIGHBOUR, 30.0);
    rotate(GD_BILINEAR_FIXED, 30.0);
    rotate(GD_BICUBIC_FIXED, 30.0);
    rotate(GD_DEFAULT, 0.0);
    rotate(GD_DEFAULT, 90.0);
    rotate(GD_DEFAULT, 180.0);
    rotate(GD_DEFAULT, 270.0);
    return gdNumFailures();
}
