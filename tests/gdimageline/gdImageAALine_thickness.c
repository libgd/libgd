#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreateTrueColor(100, 100);
	gdImageFilledRectangle(im, 0, 0, 99, 99,
			       gdImageColorExactAlpha(im, 255, 255, 255, 0));

	gdImageSetThickness(im, 5);
	gdImageSetAntiAliased(im, gdImageColorExactAlpha(im, 0, 0, 0, 0));
	gdImageLine(im, 0,0, 99, 99, gdAntiAliased);

	if (!gdAssertImageEqualsToFile("gdimageline/gdImageAALine_thickness_exp.png", im))
		error = 1;

	gdImageDestroy(im);

    return error;
}
