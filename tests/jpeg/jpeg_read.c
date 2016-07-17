#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"
#include <jpeglib.h>

int main()
{
#if defined(JPEG_LIB_VERSION_MAJOR) && JPEG_LIB_VERSION_MAJOR >= 8
	printf("skip, JPEG Major version too high (%i)\n", JPEG_LIB_VERSION_MAJOR);
	return 0;
#else
	gdImagePtr im;
	int error = 0;
	FILE *fp = gdTestFileOpen("jpeg/conv_test.jpeg");
	im = gdImageCreateFromJpeg(fp);
	fclose(fp);

	if (im == NULL) {
		gdTestErrorMsg("gdImageCreateFromJpeg failed.\n");
		return 1;
	}
	if (!gdAssertImageEqualsToFile("jpeg/conv_test_exp.png", im))
		error = 1;

	gdImageDestroy(im);
	return error;
#endif
}
