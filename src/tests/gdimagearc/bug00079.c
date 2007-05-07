#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	int error = 0;
	char path[1024];


	im = gdImageCreateTrueColor(300, 300);
	gdImageFilledRectangle(im, 0,0, 299,299, 0xFFFFFF);

	gdImageSetAntiAliased(im, 0x000000);
	gdImageArc(im, 300, 300, 600,600, 0, 360, gdAntiAliased);

	sprintf(path, "%s/gdimagearc/bug00079_exp.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		printf("%s failed\n", path);
		error = 1;
	}

	gdImageDestroy(im);
	return error;
}
