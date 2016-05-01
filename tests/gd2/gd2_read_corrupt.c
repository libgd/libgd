/* Just try to read the invalid gd2 image & not crash. */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main(int argc, char *argv[])
{
	gdImagePtr im;
	FILE *fp;
	char *inpath;

	if (argc != 2) {
		printf("Usage: %s <input gd2>\n", argv[0]);
		return 1;
	}

	/* Read the corrupt image. */
	inpath = argv[1];
	fp = fopen(inpath, "rb");
	if (!fp) {
		printf("failed, cannot open file: %s\n", inpath);
		return 1;
	}
	im = gdImageCreateFromGd2(fp);
	fclose(fp);

	/* Should have failed & rejected it. */
	return im == NULL ? 0 : 1;
}
