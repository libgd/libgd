/**
 * Test that a zero determinant matrix causes gdTransformAffineCopy() to fail
 *
 * See <https://github.com/libgd/libgd/issues/583>
 */

#include "gd.h"
#include "gdtest.h"


int main()
{
    gdImagePtr src, dst;
    gdRect rect = {0, 0, 8, 8};
    double matrix[] = {1, 1, 1, 1, 1, 1};
    int white;
    int res;

    src = gdImageCreateTrueColor(8, 8);
    gdTestAssert(src != NULL);
    dst = gdImageCreateTrueColor(8, 8);
    gdTestAssert(dst != NULL);
    white = gdImageColorAllocate(src, 255, 255, 255);
    gdImageFilledRectangle(src, 0, 0, 7, 7, white);
    res = gdTransformAffineCopy(dst, 0, 0, src, &rect, matrix);
    gdTestAssert(res == GD_FALSE);
    gdImageDestroy(dst);
    gdImageDestroy(src);
    return gdNumFailures();
}
