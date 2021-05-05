#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char *path;

	fp = gdTestFileOpen2("tiff", "tiff_read_bw.tiff");
	gdTestAssert(fp != NULL);
	im = gdImageCreateFromTiff(fp);
	fclose(fp);

	gdTestAssert(im != NULL);
	gdTestAssert(!gdImageTrueColor(im));

	path = gdTestFilePath2("tiff", "tiff_read_bw_exp.png");
	gdAssertImageEqualsToFile(path, im);
	gdFree(path);

	gdImageDestroy(im);

	return gdNumFailures();
}
