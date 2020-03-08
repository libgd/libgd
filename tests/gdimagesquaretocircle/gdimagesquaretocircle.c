/**
 * Basic test for gdImageSquareToCircle()
 */
#include "gd.h"
#include "gdfx.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im1, im2;
	int white;
	int black;

	im1 = gdImageCreateTrueColor(128, 128);
	white = gdImageColorAllocate(im1, 255, 255, 255);
	black = gdImageColorAllocate(im1, 0, 0, 0);
	gdImageFilledRectangle(im1, 0, 0, 127, 127, white);

	/* Draw a cross line in the middle of im1 */
	/* The horizonal line will become a circle in im2 */
	/* The vertical line will become a horizonal with half len in im2 */
	gdImageLine(im1, 0, 63, 127, 63, black);
	gdImageLine(im1, 63, 0, 63, 127, black);
	im2 = gdImageSquareToCircle(im1, 64);

	gdAssertImageEqualsToFile("gdimagesquaretocircle/gdimagesquaretocircle_exp.png", im2);

	gdImageDestroy(im1);
	gdImageDestroy(im2);

	return gdNumFailures();
}
