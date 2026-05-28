#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	FILE *fp;
	gdImagePtr im;
	int error = 0;

	fp = gdTestFileOpen("gif/bug000498.gif");
	im = gdImageCreateFromGif(fp);
	fclose(fp);

	if (!gdTestAssertMsg(im != NULL, "gdImageCreateFromGif failed\n")) {
		return 1;
	}

	if (!gdAssertImageEqualsToFile("gif/bug000498_exp.png", im)) {
		error = 1;
	}

	gdImageDestroy(im);

	return error;
}
