#include "gdtest.h"
#include "gd.h"

int main()
{
 	gdImagePtr im;
	const char *exp = "bug00077_exp.png";
	const int files_cnt = 4;
	FILE *fp;
	int i = 0;
	int error = 0;

	char path[1024];


	im = gdImageCreateTrueColor(11, 11);
	gdImageFilledRectangle(im, 0, 0, 10, 10, 0xFFFFFF);
	gdImageSetThickness(im, 1);

	gdImageLine(im, 0, 10, 0, 0, 0x0);
	gdImageLine(im, 5, 10, 5, 0, 0x0);
	gdImageLine(im, 10, 5, 0, 5, 0x0);
	gdImageLine(im, 10, 10, 0, 10, 0x0);

	sprintf(path, "%s/gdimageline/%s", GDTEST_TOP_DIR, exp);

	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
	}

	gdImageDestroy(im);

	return error;
}
