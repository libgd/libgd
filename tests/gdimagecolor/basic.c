/**
 * Basic test for gdImageColor()
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char *path;

	fp = gdTestFileOpen2("gdimagecolor", "basic.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	gdImageColor(im, 127, -127, -127, 0);

	path = gdTestFilePath2("gdimagecolor", "basic_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
