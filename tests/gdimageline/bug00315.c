/**
 * Regression test for <https://github.com/libgd/libgd/issues/315>
 *
 * We're testing that a single-pointed gdImageAALine() is drawn as a single
 * non-antialized pixel, according to (two-pointed) vertical and horizontal
 * lines.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int white, black;
	char *path;

	im = gdImageCreateTrueColor(6, 6);
	white = gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im, 0, 0, 0);
	gdImageFilledRectangle(im, 0,0, 5,5, white);

	gdImageLine(im, 4,4, 4,4, black);
	gdImageLine(im, 1,4, 2,4, black);
	gdImageLine(im, 4,1, 4,2, black);

	gdImageSetAntiAliased(im, black);
	gdImageLine(im, 1,1, 1,1, gdAntiAliased);

	path = gdTestFilePath2("gdimageline", "bug00315_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
