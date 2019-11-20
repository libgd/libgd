/**
 * Basic test for gdImageSquareToCircle()
 */
#include "gd.h"
#include "gdfx.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im1, im2;
	int r = 50, result = 0;

	im1 = gdImageCreateTrueColor(r * 2, r * 2);

	if (!im1) {
		gdTestErrorMsg("could not create im1\n");
		return 1;
	}

	gdImageColor(im1, 127, 127, 127, 0);
	im2 = gdImageSquareToCircle(im1, r);

	if (!im2) {
		result = 1;
		goto done;
	}

	if (!gdAssertImageEqualsToFile("gdimagesquaretocircle/circle_exp.png", im2)) {
		result = 1;
	}

done:
	gdImageDestroy(im1);
	gdImageDestroy(im2);
	return result;
}
