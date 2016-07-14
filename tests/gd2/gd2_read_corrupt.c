/* Just try to read the invalid gd2 image & not crash. */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main(int argc, char *argv[])
{
	gdImagePtr im;
	FILE *fp;

	fp = gdTestFileOpen2("gd2", "invalid_header.gd2");

	if (!fp) {
		printf("failed, cannot open file: %s\n", inpath);
		return 1;
	}
	im = gdImageCreateFromGd2(fp);
	fclose(fp);

	if (im != NULL) {
		gdTestErrorMsg("image should have failed to be loaded\n");
		gdImageDestroy(im);
		return 1;
	} else {
		return 0;
	}
}
