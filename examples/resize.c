#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
	FILE *fp;
	gdImagePtr in, out;
	int w, h;

	/* Help */
	if (argc<=4) {
		printf("%s  input.jpg  output.jpg  width  height\n", argv[0]);
		return 1;
	}

	/* Size */
	w = atoi(argv[3]);
	h = atoi(argv[4]);
	if (w<=0 || h<=0) {
		fprintf(stderr, "Bad size %dx%d\n", h, w);
		return 2;
	}

	/* Input */
	fp = fopen(argv[1], "rb");
	if (!fp) {
		fprintf(stderr, "Can't read image %s\n", argv[1]);
		return 3;
	}
	in = gdImageCreateFromJpeg(fp);
	fclose(fp);
	if (!in) {
		fprintf(stderr, "Can't create image from %s\n", argv[1]);
		return 4;
	}

	/* Resize */
	gdImageSetInterpolationMethod(in, GD_BILINEAR_FIXED);
	out = gdImageScale(in, w, h);
	if (!out) {
		fprintf(stderr, "gdImageScale fails\n");
		return 5;
	}

	/* Output */
	fp = fopen(argv[2], "wb");
	if (!fp) {
		fprintf(stderr, "Can't save image %s\n", argv[2]);
		return 6;
	}
	gdImageJpeg(out, fp, 90);
	fclose(fp);

	/* Cleanups */
	gdImageDestroy(in);
	gdImageDestroy(out);

	return 0;
}
