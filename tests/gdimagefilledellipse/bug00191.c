#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreate(100, 100);
	(void)gdImageColorAllocate(im, 255, 255, 255);
	gdImageSetThickness(im, 20);
	gdImageFilledEllipse(im, 30, 50, 20, 20, gdImageColorAllocate(im, 0, 0, 0));
	if (!gdAssertImageEqualsToFile("gdimagefilledellipse/bug00191.png", im)) {
		error = 1;
	}
	gdImageDestroy(im);
	return error;
}
