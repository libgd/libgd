/**
 * Test reading of an X10 style XBM file
 *
 * X10 style XBMs define a short[] instead of a char[] array. We're testing
 * that such files are read correctly, particularly regarding the line padding,
 * which is 2 bytes instead of 1, and the endianess.
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char *path;

	fp = gdTestFileOpen2("xbm", "x10_basic_read.xbm");
	im = gdImageCreateFromXbm(fp);
	fclose(fp);
	gdTestAssert(im != NULL);

	path = gdTestFilePath2("xbm", "x10_basic_read_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
