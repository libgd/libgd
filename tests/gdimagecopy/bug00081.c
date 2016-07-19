#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im, im2;
	int error = 0;

	im = gdImageCreateTrueColor(5, 5);
	if (!im) {
		gdTestErrorMsg("can't create the src truecolor image\n");
		return 1;
	}

	gdImageFilledRectangle(im, 0, 0, 49, 49, 0x00FFFFFF);
	gdImageColorTransparent(im, 0xFFFFFF);
	gdImageFilledRectangle(im, 1, 1, 4, 4, 0xFF00FF);

	im2 = gdImageCreateTrueColor(20, 20);
	if (!im2) {
		gdTestErrorMsg("can't create the dst truecolor image\n");
		gdImageDestroy(im);
		return 1;
	}

	gdImageCopy(im2, im, 2, 2 , 0, 0, gdImageSX(im), gdImageSY(im));

	if (!gdAssertImageEqualsToFile("gdimagecopy/bug00081_exp.png", im2)) {
		error = 1;
		gdTestErrorMsg("Reference image and destination differ\n");
	}

	gdImageDestroy(im);
	gdImageDestroy(im2);
	return error;
}
