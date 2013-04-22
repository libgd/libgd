/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	char path[1024];

	gdSetErrorMethod(gdSilence);

	sprintf(path, "%s/jpeg/empty.jpeg", GDTEST_TOP_DIR);
	fp = fopen(path, "rb");
	if (!fp) {
		printf("failed, cannot open file\n");
		return 1;
	}

	im = gdImageCreateFromJpeg(fp);
	fclose(fp);

	if (!im) {
		return 0;
	} else {
		gdImageDestroy(im);
		return 1;
	}
}
