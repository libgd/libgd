#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreateTrueColor(10, 10);
	if (!im) {
		gdTestErrorMsg("can't get truecolor image\n");
		return 1;
	}

	gdImageLine(im, 2, 2, 2, 2, 0xFFFFFF);
	gdImageLine(im, 5, 5, 5, 5, 0xFFFFFF);

	gdImageLine(im, 0, 0, 0, 0, 0xFFFFFF);

	if (!gdAssertImageEqualsToFile("gdimageline/bug00111_exp.png", im)) {
		error = 1;
		gdTestErrorMsg("Reference image and destination differ\n");
	}

	gdImageDestroy(im);

	return error;
}
