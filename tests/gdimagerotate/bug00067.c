#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im, exp;
	char path[2048];
	const char *file_im = "gdimagerotate/remirh128.jpg";
	const char *file_exp = "gdimagerotate/bug00067";
	FILE *fp;
	int color;
	int error = 0;
	int angle;

	sprintf(path, "%s/%s", GDTEST_TOP_DIR, file_im);

	fp = fopen(path, "rb");

	if (!fp) {
		gdTestErrorMsg("opening Jpeg %s for reading failed.\n", path);
		return 1;
	}

	im = gdImageCreateFromJpeg(fp);

	fclose(fp);

	if (!im) {
		gdTestErrorMsg("loading %s failed.\n", path);
		return 1;
	}

	color = gdImageColorAllocate(im, 0, 0, 0);

	if (color < 0) {
		gdTestErrorMsg("allocation color from image failed.\n");
		gdImageDestroy(im);
		return 1;
	}

	for (angle = 0; angle <= 180; angle += 15) {

		exp = gdImageRotateInterpolated(im, angle, color);

		if (!exp) {
			gdTestErrorMsg("rotating image failed for %03d.\n", angle);
			gdImageDestroy(im);
			return 1;
		}

		sprintf(path, "%s/%s_%03d_exp.png", GDTEST_TOP_DIR, file_exp, angle);

		if (!gdAssertImageEqualsToFile(path, exp)) {
			gdTestErrorMsg("comparing rotated image to %s failed.\n", path);
			error += 1;
		}

		gdImageDestroy(exp);
	}

	gdImageDestroy(im);

	return error;
}
