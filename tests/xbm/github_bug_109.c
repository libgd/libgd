/**
 * Test reading of XBM images with a width that is not a multiple of 8
 *
 * We're reading such an XBM image, and check that we got what we've expected,
 * instead of an error message.
 *
 * See also <https://github.com/libgd/libgd/issues/109>.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char *path;

	fp = gdTestFileOpen2("xbm", "github_bug_109.xbm");
	im = gdImageCreateFromXbm(fp);
	fclose(fp);
	gdTestAssert(im != NULL);
	gdTestAssert(gdImageGetTrueColorPixel(im, 0, 0) == 0);
	gdTestAssert(gdImageGetTrueColorPixel(im, 0, 1) == 0xffffff);

	path = gdTestFilePath2("xbm", "github_bug_109_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
