/* $Id$ */

#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"
#include "gdfontl.h"

int main()
{
	gdImagePtr im;
	int error = 0;
	char path[2048];
	const char *file_exp = "bug00132_exp.png";
	char *ret = NULL;

	im = gdImageCreateTrueColor(50, 30);

	if (!im) {
		printf("can't get truecolor image\n");
		return 1;
	}

	gdImageAlphaBlending(im, 0);
	gdImageFilledRectangle(im, 0, 0, 200, 200, gdTrueColorAlpha(0, 0, 0, 127));

	sprintf(path, "%s/freetype/DejaVuSans.ttf", GDTEST_TOP_DIR);

	ret = gdImageStringFT(im, NULL,  - 0xFFFFFF, path, 14.0, 0.0, 10, 20, "&thetasym; &theta;");
	if (ret) {
		error = 1;
		printf("%s\n", ret);
	} else {
		sprintf(path, "%s/freetype/%s", GDTEST_TOP_DIR, file_exp);
		if (!gdAssertImageEqualsToFile(path, im)) {
			error = 1;
			printf("Reference image and destination differ\n");
		}
	}
	gdImageDestroy(im);
	return error;
}
