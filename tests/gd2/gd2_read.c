/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main(int argc, char *argv[])
{
	int error;
	gdImagePtr im;
	FILE *fp;
	char *inpath, *exppath;

	if (argc != 3) {
		printf("Usage: %s <input gd2> <expected png>\n", argv[0]);
		return 1;
	}

	inpath = argv[1];
	exppath = argv[2];

	fp = fopen(inpath, "rb");
	if (!fp) {
		printf("failed, cannot open file: %s\n", inpath);
		return 1;
	}

	im = gdImageCreateFromGd2(fp);
	fclose(fp);

	if (!gdAssertImageEqualsToFile(exppath, im)) {
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
