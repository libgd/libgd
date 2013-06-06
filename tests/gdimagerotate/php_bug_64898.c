#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im, exp;
	char path[2048];
	const char *file_im = "gdimagerotate/php_bug_64898.png";
	const char *file_exp = "gdimagerotate/php_bug_64898_exp.png";
	FILE *fp;
	int color;

	sprintf(path, "%s/%s", GDTEST_TOP_DIR, file_im);

	fp = fopen(path, "rb");

	if (!fp) {
		gdTestErrorMsg("opening PNG %s for reading failed.\n", path);
		return 1;
	}

	im = gdImageCreateTrueColor(141, 200);

	if (!im) {
		gdTestErrorMsg("loading %s failed.\n", path);
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

	sprintf(path, "%s/%s", GDTEST_TOP_DIR, file_exp);

	if (!gdAssertImageEqualsToFile(path, exp)) {
		printf("comparing rotated image to %s failed.\n", path);
		gdImageDestroy(im);
		gdImageDestroy(exp);
		return 1;
	}

	gdImageDestroy(exp);
	gdImageDestroy(im);

	return 0;
}
