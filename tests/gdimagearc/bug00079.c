#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreateTrueColor(300, 300);
	gdImageFilledRectangle(im, 0,0, 299,299, 0xFFFFFF);

	gdImageSetAntiAliased(im, 0x000000);
	gdImageArc(im, 300, 300, 600,600, 0, 360, gdAntiAliased);

	if (!gdAssertImageEqualsToFile("gdimagearc/bug00079_exp.png", im)) {
		error = 1;
	}

	gdImageDestroy(im);
	return error;
}
