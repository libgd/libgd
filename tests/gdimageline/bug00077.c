#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreateTrueColor(11, 11);
	gdImageFilledRectangle(im, 0, 0, 10, 10, 0xFFFFFF);
	gdImageSetThickness(im, 1);

	gdImageLine(im, 0, 10, 0, 0, 0x0);
	gdImageLine(im, 5, 10, 5, 0, 0x0);
	gdImageLine(im, 10, 5, 0, 5, 0x0);
	gdImageLine(im, 10, 10, 0, 10, 0x0);

	if (!gdAssertImageEqualsToFile("gdimageline/bug00077_exp.png", im)) {
		error = 1;
	}

	gdImageDestroy(im);

	return error;
}
