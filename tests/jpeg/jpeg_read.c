/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"
#include <jpeglib.h>

#ifdef _MSC_VER
# define snprintf _snprintf
#endif

int main()
{
#if defined(JPEG_LIB_VERSION_MAJOR) && JPEG_LIB_VERSION_MAJOR >= 8
	printf("skip, JPEG Major version too high (%i)\n", JPEG_LIB_VERSION_MAJOR);
	return 0;
#else
	gdImagePtr im;
	FILE *fp;
	char path[1024];

	snprintf(path, sizeof(path)-1, "%s/jpeg/conv_test.jpeg", GDTEST_TOP_DIR);
	fp = fopen(path, "rb");
	if (!fp) {
		gdTestErrorMsg("failed, cannot open file: %s\n", path);
		return 1;
	}

	im = gdImageCreateFromJpeg(fp);
	fclose(fp);

	if (im == NULL) {
		gdTestErrorMsg("gdImageCreateFromJpeg failed.\n");
		return 1;
	}
	snprintf(path, sizeof(path), "%s/jpeg/conv_test_exp.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		gdTestErrorMsg("gdAssertImageEqualsToFile failed: <%s>.\n", path);
		gdImageDestroy(im);
		return 1;
	}

	return 0;
#endif
}
