/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	int error;
	gdImagePtr im;
	FILE *fp;
	char path[1024];

	sprintf(path, "%s/gd2/conv_test.gd2", GDTEST_TOP_DIR);

	fp = fopen(path, "rb");
	if (!fp) {
		printf("failed, cannot open file\n");
		return 1;
	}

	im = gdImageCreateFromGd2(fp);
	fclose(fp);

	sprintf(path, "%s/gd2/conv_test_exp.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
		gdImageDestroy(im);
	} else {
		if (im) {
			gdImageDestroy(im);
			error = 0;
		} else {
			error = 1;
		}
	}
	return error;
}
