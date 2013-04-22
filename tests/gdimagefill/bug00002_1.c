#include "gd.h"
#include <stdio.h>
#include "gdtest.h"

#define TMP_FN "bug00002_1.png"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char path[1024];

	im = gdImageCreateTrueColor(100, 100);

	if (im == NULL) {
		gdTestErrorMsg("Cannot create image.\n");
		return 1;
	}

	gdImageFill(im, 0, 0, 0xffffff);
	gdImageFill(im, 0, 0, 0xffffff);

	fp = fopen(TMP_FN, "wb");
	if (fp == NULL) {
		gdTestErrorMsg("Cannot create image from <%s>\n", TMP_FN);
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

	if (remove(TMP_FN) == -1) {
		gdTestErrorMsg("Cannot remove temporary file: <%s>\n", TMP_FN);
		return 1;
	}

	return 0;
}

