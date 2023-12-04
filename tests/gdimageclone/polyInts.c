/**
 * Cloning polyInts without crashing
 */

#include <string.h>
#include "gd.h"
#include "gdtest.h"


int main()
{
	gdImagePtr im, clone;
	gdPoint pts[] = { {5, 5}, {50, 8}, {80, 4}, {85, 50}, {10, 60} };

	im = gdImageCreateTrueColor(100, 100);
	gdImageFilledPolygon(im, pts, 3, gdTrueColor(255, 0, 0));
	clone = gdImageClone(im);

	gdTestAssert(clone != NULL);
	gdTestAssert(clone->polyAllocated == im->polyAllocated);
	gdTestAssert(clone->polyInts != NULL);
	gdTestAssert(!memcmp(clone->polyInts, im->polyInts, clone->polyAllocated * sizeof(clone->polyInts[0])));

	/* test for reallocating clone->polyInts with glImageFilledPolygon */
	gdImageFilledPolygon(clone, pts, 5, gdTrueColor(255, 0, 255));
	gdTestAssert(clone->polyAllocated >= 5);

	gdImageDestroy(clone);
	gdImageDestroy(im);

	return gdNumFailures();
}
