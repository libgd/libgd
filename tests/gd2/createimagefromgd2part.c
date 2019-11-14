#include "gd.h"
#include "gdtest.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	gdImagePtr im;
	FILE *p;

	p = gdTestFileOpen2("gd2", "conv_test.gd2");

	if (!p) {
		gdTestErrorMsg("failed, connot open gd2 file: conv_test.gd2");
		return 1;
	}

	im = gdImageCreateFromGd2Part(p, 5, 5, 5, 5);
	fclose(p);

	if (!im) {
		return 1;
	}

	gdImageDestroy(im);
	return 0;
}
