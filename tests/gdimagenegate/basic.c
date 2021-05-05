/**
 * Basic test for gdImageNegate()
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char *path;

	fp = gdTestFileOpen2("gdimagenegate", "basic.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	gdImageNegate(im);

	path = gdTestFilePath2("gdimagenegate", "basic_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
