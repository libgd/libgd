/*
	Test writing of XBM images with a width that is not a multiple of 8

	We create an image with a width of 11 pixels, and draw a circle on it.
	To test that the padding is correctly applied, we write the image to disk
	and assert that the number of bytes written matches our expectation.

	See also <https://github.com/libgd/libgd/issues/170>.
*/

#include <inttypes.h>
#include "gd.h"
#include "gdtest.h"
#ifdef _WIN32

int main()
{
	/* skip for now */
	return 0;
}
#else
int main()
{
	gdImagePtr im;
	int black;
	FILE *outFile;
	gdIOCtx *out;
	off_t length;

	/* create the test image */
	im = gdImageCreate(11, 11);
	gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im, 0, 0, 0);
	gdImageArc(im, 5, 5, 10, 10, 0, 360, black);

	/* write the file to disk, note the file length and delete the file */
	outFile = gdTestTempFp();
	out = gdNewFileCtx(outFile);
	gdTestAssert(out != NULL);
	gdImageXbmCtx(im, "github_bug_170.xbm", 1, out);
	out->gd_free(out);
	length = ftello(outFile);
	fclose(outFile);

	gdImageDestroy(im);

	gdTestAssertMsg(length == 250, "expected to write 250 bytes; %jd bytes written", (intmax_t) length);
	return gdNumFailures();
}
#endif
