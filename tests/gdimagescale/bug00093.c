/**
 * Regression test for <https://github.com/libgd/libgd/issues/93>
 *
 * We're drawing a black rectangle along the border of a white image, and
 * check that sample points near the corners of the scaled image have a red
 * channel value below a certain threshold, i.e. are gray and not white. We
 * don't check against exact values to avoid potential precision issues as
 * floating point arithmethic is involved in the scaling algorithm.
 */


#include "gd.h"
#include "gdtest.h"


static void check_upscaled_pixel(gdImagePtr im, int x, int y)
{
    int red = gdTrueColorGetRed(gdImageGetPixel(im, x, y));
    
    gdTestAssertMsg(red < 128, "pixel %d,%d: expected red < 128, but got %d", x, y, red);
}


static void check_upscale()
{
    gdImagePtr src, dst;

    src = gdImageCreateTrueColor(11, 11);
    gdImageFilledRectangle(src, 0, 0, 10, 10, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));
    gdImageRectangle(src, 0, 0, 10, 10, gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque));

    gdImageSetInterpolationMethod(src, GD_TRIANGLE);
    dst = gdImageScale(src, 132, 132);

    check_upscaled_pixel(dst, 11, 11);
    check_upscaled_pixel(dst, 11, 120);
    check_upscaled_pixel(dst, 120, 11);
    check_upscaled_pixel(dst, 120, 120);

    gdImageDestroy(src);
    gdImageDestroy(dst);
}


static void check_downscaled_pixel(gdImagePtr im, int x, int y)
{
    int red = gdTrueColorGetRed(gdImageGetPixel(im, x, y));
    
    gdTestAssertMsg(red < 240, "pixel %d,%d: expected red < 240, but got %d", x, y, red);
}


static void check_downscale()
{
    gdImagePtr src, dst;

    src = gdImageCreateTrueColor(132, 132);
    gdImageFilledRectangle(src, 0, 0, 131, 131, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));
    gdImageRectangle(src, 0, 0, 131, 131, gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque));

    gdImageSetInterpolationMethod(src, GD_TRIANGLE);
    dst = gdImageScale(src, 11, 11);

    check_downscaled_pixel(dst, 0, 0);
    check_downscaled_pixel(dst, 0, 10);
    check_downscaled_pixel(dst, 10, 0);
    check_downscaled_pixel(dst, 10, 10);

    gdImageDestroy(src);
    gdImageDestroy(dst);
}


int main()
{
    check_upscale();
    check_downscale();

    return gdNumFailures();
}
