#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreateTrueColor(100,100);
	gdImageFilledEllipse(im, 50,50, 70, 90, 0x50FFFFFF);

	if (!gdAssertImageEqualsToFile("gdimagefilledellipse/bug00010_exp.png", im)) {
		error = 1;
	}

	gdImageDestroy(im);
	return error;
}
