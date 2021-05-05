/**
 * Basic test for writing and reading lossless WebP
 *
 * We create an image, write it as WebP, read it in again, and compare it
 * with the original image.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst;
	int r, g, b;
	void *p;
	int size = 0;

	src = gdImageCreateTrueColor(100, 100);
	gdTestAssert(src != NULL);
	r = gdImageColorAllocate(src, 0xFF, 0, 0);
	g = gdImageColorAllocate(src, 0, 0xFF, 0);
	b = gdImageColorAllocate(src, 0, 0, 0xFF);
	gdImageFilledRectangle(src, 0, 0, 99, 99, r);
	gdImageRectangle(src, 20, 20, 79, 79, g);
	gdImageEllipse(src, 75, 25, 30, 20, b);

	p = gdImageWebpPtrEx(src, &size, gdWebpLossless);
	gdTestAssert(p != NULL);
	gdTestAssert(size > 0);

	dst = gdImageCreateFromWebpPtr(size, p);
	gdTestAssert(dst != NULL);

	gdAssertImageEquals(src, dst);

	gdFree(p);
	gdImageDestroy(dst);
	gdImageDestroy(src);

	return gdNumFailures();
}
