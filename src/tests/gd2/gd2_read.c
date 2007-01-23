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

	fp = fopen("conv_test.gd2", "rb");
	if (!fp) {
		printf("failed, cannot open file\n");
		return 1;
	}

	im = gdImageCreateFromGd2(fp);
	fclose(fp);

	if (!gdAssertImageEqualsToFile("conv_test_exp.png", im)) {
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
