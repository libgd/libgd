/**
 * Base test for gdImageCreateFromGd2PartPtr()
 */
#include "gd.h"
#include "gdtest.h"
#include <stdio.h>

int main()
{
	FILE *p;
	gdImagePtr im, partim;
	void *pg;
	int size = 0;
	int status = 0;
	int actual_color = 0;
	int expected_color = 0xffffff;

	p = gdTestFileOpen2("gd2", "conv_test.gd2");
	if (!p) {
		gdTestErrorMsg("failed, cannot open gd2 file:conv_test.gd2");
		return 1;
	}

	im = gdImageCreateFromGd2(p);
	fclose(p);

	if (!im) {
		gdTestErrorMsg("failed, cannot create gd2 file.");
		return 1;
	}

	pg = gdImageGd2Ptr(im, (GD2_CHUNKSIZE_MIN + GD2_CHUNKSIZE_MAX) / 2, GD2_FMT_COMPRESSED, &size);
	if (!pg) {
		status = 1;
		goto done1;
	}

	if (size <= 0) {
		status = 1;
		goto done0;
	}

	partim =  gdImageCreateFromGd2PartPtr(size, pg, 3, 3, 3, 3);
	if (!partim) {
		status = 1;
		goto done0;
	}

	actual_color = gdImageGetPixel(partim, 2, 2);
	status = (expected_color == actual_color) ? 0 : 1;
	gdImageDestroy(partim);

done0:
	gdFree(pg);
done1:
	gdImageDestroy(im);

	return status;
}
