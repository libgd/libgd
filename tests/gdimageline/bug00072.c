#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreateTrueColor(11, 11);
	gdImageFilledRectangle(im, 0, 0, 10, 10, 0xFFFFFF);
	gdImageSetThickness(im, 3);

	gdImageLine(im, 5, 0, 5, 11, 0x000000);
	gdImageLine(im, 0, 5, 11, 5, 0x000000);
	gdImageLine(im, 0, 0, 11, 11, 0x000000);

	gdImageSetThickness(im, 1);

	gdImageLine(im, 5, 0, 5, 11, 0xFF0000);
	gdImageLine(im, 0, 5, 11, 5, 0xFF0000);
	gdImageLine(im, 0, 0, 11, 11, 0xFF0000);

	if (!gdAssertImageEqualsToFile("gdimageline/bug00072_exp.png", im)) {
		error = 1;
	}

	gdImageDestroy(im);

	return error;
}
