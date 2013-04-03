/* $Id$ */

#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;
	char path[2048];
	const char *file_exp = "bug00111_exp.png";

	im = gdImageCreateTrueColor(10, 10);
	if (!im) {
		printf("can't get truecolor image\n");
		return 1;
	}

	gdImageLine(im, 2, 2, 2, 2, 0xFFFFFF);
	gdImageLine(im, 5, 5, 5, 5, 0xFFFFFF);

	gdImageLine(im, 0, 0, 0, 0, 0xFFFFFF);

	sprintf(path, "%s/gdimageline/%s", GDTEST_TOP_DIR, file_exp);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
		printf("Reference image and destination differ\n");
	}

	gdImageDestroy(im);

	return error;
}
