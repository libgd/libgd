#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im, exp;
	char *path, filename[2048];
	const char *file_im = "gdimagerotate/remirh128.png";
	FILE *fp;
	int color;
	int error = 0;
	int angle;

	fp = gdTestFileOpen(file_im);
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	if (!im) {
		gdTestErrorMsg("loading %s failed.\n", file_im);
		return 1;
	}

	color = gdImageColorAllocateAlpha(im, 255, 255, 255, 127);

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

		sprintf(filename, "bug00067_%03d_exp.png", angle);
		path = gdTestFilePath2("gdimagerotate", filename);
		if (!gdAssertImageEqualsToFile(path, exp)) {
			gdTestErrorMsg("comparing rotated image to %s failed.\n", path);
			error += 1;
		}
		free(path);

		gdImageDestroy(exp);
	}

	gdImageDestroy(im);

	return error;
}
