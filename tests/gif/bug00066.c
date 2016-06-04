#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	int error = 0;

	fp = gdTestFileOpen("gif/bug00066.gif");
	im = gdImageCreateFromGif(fp);
	fclose(fp);

	if (!gdAssertImageEqualsToFile("gif/bug00066_exp.png", im)) {
		error = 1;
	}
	gdImageDestroy(im);

	return error;
}
