/*
 * Test that gdTransformAffineCopy() does not segfault for palette images
 *
 * See <>
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	gdRect rect = {0, 0, 8, 8};
	double matrix[] = {1, 0, 0, 1, 0, 0};

	src = gdImageCreate(8, 8);
	gdTestAssert(src != NULL);
	dst = gdImageCreate(8, 8);
	gdTestAssert(dst != NULL);

	gdTransformAffineCopy(dst, 0, 0, src, &rect, matrix);

	gdImageDestroy(dst);
	gdImageDestroy(src);
	return gdNumFailures();
}
