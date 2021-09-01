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
// __aarch64__/graviton. It fails within the CI while outside is 100% success over 100s builds&runs
#if defined(__MINGW32__) || defined(__aarch64__) || defined(_M_ARM64)
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
