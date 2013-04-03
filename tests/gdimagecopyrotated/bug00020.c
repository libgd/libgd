/* $Id$ */
#include "gd.h"
#include "gdtest.h"

#define width 50

int main()
{
	gdImagePtr im, im2;
	int error = 0;
	char path[1024];

	sprintf(path, "%s/gdimagecopyrotated/bug00020_exp.png", GDTEST_TOP_DIR);

	im = gdImageCreateTrueColor(width, width);
	gdImageFilledRectangle(im, 0,0, width, width, 0xFF0000);
	gdImageColorTransparent(im, 0xFF0000);
	gdImageFilledEllipse(im, width/2, width/2, width - 20, width - 10,
	                     0x50FFFFFF);

	im2 = gdImageCreateTrueColor(width, width);

	gdImageCopyRotated(im2, im, width / 2, width / 2, 0,0, width, width, 60);

	if (!gdAssertImageEqualsToFile(path, im2)) {
		error = 1;
	}

	gdImageDestroy(im2);
	gdImageDestroy(im);
	return error;
}
