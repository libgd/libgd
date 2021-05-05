/*
	Test reading an invalid XBM image.

	The pixels of the XBM image are invalid hex which makes the uninitialezed
	variable be encoded into the output image i.e. information disclosure.
	The image is 8*2.

	See also <https://github.com/libgd/libgd/issues/501>.
*/

#include "gd.h"
#include "gdtest.h"

int main()
{

	gdImagePtr im;
	FILE *fp;

	fp = gdTestFileOpen2("xbm", "github_bug_501.xbm");
	im = gdImageCreateFromXbm(fp);

	gdTestAssert(im == NULL);

	if (im) {
		gdTestErrorMsg("Info Disclosed\n");
		int i;
		for (i = 0; i < 8; i++) {
			printf("Pixel(%d, 0) %0x\n", i, gdImageGetPixel(im, i, 0));
		}
		for (i = 0; i < 8; i++) {
			printf("Pixel(%d, 1) %0x\n", i, gdImageGetPixel(im, i, 1));
		}
		gdImageDestroy(im);
	}

	fclose(fp);
	return gdNumFailures();
}
