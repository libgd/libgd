/**
 *Base test for gdImageCreateFromGd2Part()
 */
#include "gd.h"
#include "gdtest.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	gdImagePtr im;
	FILE *p;
	int expected_color = 0xffffff;
	int actual_color = 0;

	p = gdTestFileOpen2("gd2", "conv_test.gd2");

	if (!p) {
		gdTestErrorMsg("failed, connot open gd2 file: conv_test.gd2");
		return 1;
	}

	im = gdImageCreateFromGd2Part(p, 3, 3, 3, 3);
	fclose(p);

	if (!im) {
		return 1;
	}

	actual_color = gdImageGetPixel(im, 2, 2);
	gdImageDestroy(im);
	gdTestAssert(expected_color == actual_color);

	return gdNumFailures();
}
