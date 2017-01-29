#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"
#include "gdfontl.h"

int main()
{
	gdImagePtr im;
	int error = 0;
	char *path;
	char *ret = NULL;

	/* disable subpixel hinting */
	putenv("FREETYPE_PROPERTIES=truetype:interpreter-version=35");

	im = gdImageCreateTrueColor(50, 30);

	if (!im) {
		gdTestErrorMsg("can't get truecolor image\n");
		return 1;
	}

	gdImageAlphaBlending(im, 0);
	gdImageFilledRectangle(im, 0, 0, 200, 200, gdTrueColorAlpha(0, 0, 0, 127));

	path = gdTestFilePath("freetype/DejaVuSans.ttf");
	ret = gdImageStringFT(im, NULL,  - 0xFFFFFF, path, 14.0, 0.0, 10, 20, "&thetasym; &theta;");
	free(path);
	if (ret) {
		error = 1;
		gdTestErrorMsg("%s\n", ret);
	} else {
		if (!gdAssertImageEqualsToFile("freetype/bug00132_exp.png", im)) {
			error = 1;
			gdTestErrorMsg("Reference image and destination differ\n");
		}
	}
	gdImageDestroy(im);
	gdFontCacheShutdown();
	return error;
}
