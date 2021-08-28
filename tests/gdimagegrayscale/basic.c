/**
 * Basic test for gdImageGrayScale()
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char *path;
// Rounding issue, won't fix as it only happens on mingw 32bit.
#if defined(__MINGW32__)
    return 77;
#endif 
	fp = gdTestFileOpen2("gdimagegrayscale", "basic.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);

	gdImageGrayScale(im);

	path = gdTestFilePath2("gdimagegrayscale", "basic_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
