/**
 * Regression test for <https://github.com/libgd/libgd/issues/328>
 *
 * We're testing that a simple scale operation with GD_WEIGHTED4 succeeds.
 */


#include "gd.h"
#include "gdtest.h"


int main()
{
	gdImagePtr src, dst;

	src = gdImageCreateTrueColor(100, 100);
	gdImageFilledRectangle(src, 0,0, 99,99, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));

	gdImageSetInterpolationMethod(src, GD_WEIGHTED4);
	dst = gdImageScale(src, 200, 200);
	gdTestAssertMsg(dst != NULL, "scaling failed");

	gdImageDestroy(src);
	gdImageDestroy(dst);

	return gdNumFailures();
}
