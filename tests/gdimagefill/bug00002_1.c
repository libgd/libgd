#include "gd.h"
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char *file;
	char path[1024];

	im = gdImageCreateTrueColor(100, 100);

	if (im == NULL) {
		gdTestErrorMsg("Cannot create image.\n");
		return 1;
	}

	gdImageFill(im, 0, 0, 0xffffff);
	gdImageFill(im, 0, 0, 0xffffff);

	file = gdTestTempFile("bug00002_1.png");
	fp = fopen(file, "wb");
	if (fp == NULL) {
		gdTestErrorMsg("Cannot create image from <%s>\n", file);
		gdImageDestroy(im);
		return 1;
	}

	gdImagePng(im,fp);
	fclose(fp);

	sprintf(path, "%s/gdimagefill/bug00002_1_exp.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		gdImageDestroy(im);
		return 1;
	}

	gdImageDestroy(im);

	return 0;
}

