#include <limits.h>
#include "gd.h"
#include "gdhelpers.h"
#include "gdtest.h"


int main()
{
    gdImagePtr dst = gdImageCreateTrueColor(2, 2);
    gdImagePtr src = gdImageCreateTrueColor(2, 2);
    if (!dst || !src) return 2;

    int black = gdImageColorAllocate(dst, 0, 0, 0);
    int white = gdImageColorAllocate(src, 255, 255, 255);

    gdImageFilledRectangle(dst, 0, 0, 1, 1, black);
    gdImageFilledRectangle(src, 0, 0, 1, 1, white);

    /* dstX is chosen so that dstX + w overflows signed int */
    int dstX = INT_MAX - 5;
    int dstY = 0;

    //gdImageCopyRotated(dst, src, dstX, dstY, 0, 0, 10, 10, 45.0);
	gdImageCopyRotated(dst, src, 0, 0, INT_MAX - 5, INT_MAX - 5, 10, 10, 45.0);
	int p = gdImageGetPixel(dst, 0, 0);
    gdTestAssertMsg(p==0, "Expected pixel to be unchanged when drawing ellipse with dimensions that would cause overflow\n");
    gdImageDestroy(dst);
    gdImageDestroy(src);
    return 0;
}
