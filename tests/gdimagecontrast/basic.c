/**
 * Basic test for gdImageContrast()
 */

#include "gd.h"
#include "gdtest.h"

static void test_contrast(double contrast);

int main()
{
	test_contrast(+50.0);
	test_contrast(-50.0);

	return gdNumFailures();
}

static void test_contrast(double contrast)
{
	gdImagePtr im;
	FILE *fp;
	char basename[256];
	char *path;

	fp = gdTestFileOpen2("gdimagecontrast", "basic.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	gdImageContrast(im, contrast);

	sprintf(basename, "basic%+03.0f.png", contrast);
	path = gdTestFilePath2("gdimagecontrast", basename);
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);
}
