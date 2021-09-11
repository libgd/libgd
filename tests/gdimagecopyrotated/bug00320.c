#include "gd.h"
#include "gdtest.h"

#define width 20

int main()
{
    gdImagePtr src, dst;
    int black;

    src = gdImageCreate(10, 10);
    black = gdImageColorAllocate(src, 0, 0, 0);

    gdTestAssert(gdImageTrueColor(src) == 0);
    dst = gdImageRotateInterpolated(src, 30.0, black);
    gdTestAssert(dst != NULL);
    gdTestAssert(gdImageTrueColor(src) == 0);

	gdImageDestroy(src);
	gdImageDestroy(dst);
	return gdNumFailures();
}
