#include "gdtest.h"
#include "gd.h"

int main()
{
 	gdImagePtr im;
	const char *exp = "bug00072_exp.png";
	const int files_cnt = 4;
	FILE *fp;
	int i = 0;
	int error = 0;

	char path[1024];


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

	sprintf(path, "%s/gdimageline/%s", GDTEST_TOP_DIR, exp);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
	}

	gdImageDestroy(im);

	return error;
}
