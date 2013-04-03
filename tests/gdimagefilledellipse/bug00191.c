/* $Id$ */
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;
	char path[1024];

	im = gdImageCreate(100, 100);
	(void)gdImageColorAllocate(im, 255, 255, 255);
	gdImageSetThickness(im, 20);
	gdImageFilledEllipse(im, 30, 50, 20, 20, gdImageColorAllocate(im, 0, 0, 0));
	sprintf(path, "%s/gdimagefilledellipse/bug00191.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
	}
	gdImageDestroy(im);
	return error;
}
