#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main(int argc, char **argv)
{
	gdImagePtr im;
	char path[2048];
	const char *file_exp = "gdimageline/gdImageAALine_thickness_exp.png";

	im = gdImageCreateTrueColor(100, 100);
	gdImageFilledRectangle(im, 0, 0, 99, 99,
			       gdImageColorExactAlpha(im, 255, 255, 255, 0));

	gdImageSetThickness(im, 5);
	gdImageSetAntiAliased(im, gdImageColorExactAlpha(im, 0, 0, 0, 0));
	gdImageLine(im, 0,0, 99, 99, gdAntiAliased);

	sprintf(path, "%s/%s", GDTEST_TOP_DIR, file_exp);

	if (!gdAssertImageEqualsToFile(path, im)) {
		printf("comparing rotated image to %s failed.\n", path);
		gdImageDestroy(im);
		return 1;
	}

	gdImageDestroy(im);
}
