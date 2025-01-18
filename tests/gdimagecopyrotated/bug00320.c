/*
 * Test that gdImageRotateInterpolated() never converts the src to palette.
 */

#include "gd.h"
#include "gdtest.h"

static void rotate(int method, float angle)
{
    gdImagePtr src = NULL, dst = NULL;
    int black;

    src = gdImageCreate(10, 10);
    if (!gdTestAssert(src != NULL)) {
        goto out;
    }
    black = gdImageColorAllocate(src, 0, 0, 0);

    gdTestAssert(gdImageTrueColor(src) == 0);
    gdImageSetInterpolationMethod(src, method);
    dst = gdImageRotateInterpolated(src, angle, black);
    if (!gdTestAssert(dst != NULL)) {
        goto out;
    }
    gdTestAssert(gdImageTrueColor(src) == 0);
    if (!gdTestAssert(dst != src)) {
        // original image had been returned
        dst = NULL;
    }

out:
    if (src != NULL) {
        gdImageDestroy(src);
    }
    if (dst != NULL) {
        gdImageDestroy(dst);
    }
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
