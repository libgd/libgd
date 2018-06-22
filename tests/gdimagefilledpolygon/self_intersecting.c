/**
 * We're testing the filling behavior of self-intersecting polygons, which is
 * *currently* using the even-odd fillrule.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int black;
	gdPoint points[] = {{50, 5}, {24, 86}, {93, 36}, {7, 36}, {76, 86}};
	char *path;

	im = gdImageCreate(100, 100);
	gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im, 0, 0, 0);

	gdImageFilledPolygon(im, points, 5, black);

	path = gdTestFilePath2("gdimagefilledpolygon", "self_intersecting_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
