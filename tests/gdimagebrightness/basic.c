/**
 * Basic test for gdImageBrightness()
 */

#include "gd.h"
#include "gdtest.h"

static void test_brightness(int brightness);

int main()
{
	test_brightness(+100);
	test_brightness(-100);

	return gdNumFailures();
}

static void test_brightness(int brightness)
{
	gdImagePtr im;
	FILE *fp;
	char basename[256];
	char *path;

	fp = gdTestFileOpen2("gdimagebrightness", "basic.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	gdImageBrightness(im, brightness);

	sprintf(basename, "basic%+03d.png", brightness);
	path = gdTestFilePath2("gdimagebrightness", basename);
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);
}
