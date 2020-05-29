/**
 * Test gdTransformAffineCopy() run error
 *
 * See <https://github.com/libgd/libgd/issues/596>
 *
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	double matrix[] = {1, 0, 0, 1, 0, 0};
	gdImagePtr src, dst;
	gdRect rect = {0, 0, 16, 16};
	int white, green;
	int status = 0;
	int res;
	int actual_color = 0;
	int expected_color = 0x00ff00;

	src = gdImageCreateTrueColor(16, 16);
	gdTestAssert(src != NULL);
	white = gdImageColorAllocate(src, 255, 255, 255);
	gdImageFilledRectangle(src, 0, 0, 16, 16, white);

	dst = gdImageCreateTrueColor(50, 50);
	gdTestAssert(dst != NULL);
	green = gdImageColorAllocate(dst, 0, 255, 0);
	gdImageFilledRectangle(dst, 0, 0, 50, 50, green);

	res = gdTransformAffineCopy(dst, 4, 4, src, &rect, matrix);
	gdTestAssert(res != GD_FALSE);

	status = gdNumFailures();
	actual_color = gdImageGetPixel(dst, 20, 5);
	status = (actual_color == expected_color) ? status : 1;

	gdImageDestroy(src);
	gdImageDestroy(dst);
	return status;
}
