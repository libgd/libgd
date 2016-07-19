#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im, exp;
	FILE *fp;
	int error = 0;

	fp = gdTestFileOpen("gdimagerotate/php_bug_64898.png");
(void)fp;
	im = gdImageCreateTrueColor(141, 200);

	if (!im) {
		gdTestErrorMsg("loading failed.\n");
		return 1;
	}

	gdImageFilledRectangle(im, 0, 0, 140, 199, 0x00ffffff);

/*	Try default interpolation method, but any non-optimized fails */
/*	gdImageSetInterpolationMethod(im, GD_BICUBIC_FIXED); */

	exp = gdImageRotateInterpolated(im, 45, 0x0);

	if (!exp) {
		gdTestErrorMsg("rotating image failed.\n");
		gdImageDestroy(im);
		return 1;
	}

	if (!gdAssertImageEqualsToFile("gdimagerotate/php_bug_64898_exp.png", exp)) {
		gdTestErrorMsg("comparing rotated image failed.\n");
		error = 1;
	}

	gdImageDestroy(exp);
	gdImageDestroy(im);

	return error;
}
