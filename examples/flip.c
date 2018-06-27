#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
gdImagePtr loadImage(const char *name)
{
	FILE *fp;
	gdImagePtr im;

	fp = fopen(name, "rb");
	if (!fp) {
		fprintf(stderr, "Can't open jpeg file\n");
		return NULL;
	}

	im = gdImageCreateFromJpeg(fp);
	fclose(fp);
	return im;
}

int savePngImage(gdImagePtr im, const char *name)
{
	FILE *fp;
	fp = fopen(name, "wb");
	if (!fp) {
		fprintf(stderr, "Can't save png image fromtiff.png\n");
		return 0;
	}
	gdImagePng(im, fp);
	fclose(fp);
	return 1;
}

int main(int argc, char **arg)
{
	gdImagePtr im;
	int returncode = 0;

	if (argc < 2) {
		fprintf(stderr, "Usage: flip [filename.png]\n");
		return 1;
	}

	im = loadImage(arg[1]);
	if (!im) goto error;
	gdImageFlipHorizontal(im);
	if (!savePngImage(im, "flip_horizontal.png")) {
		goto error;
	}
	gdImageDestroy(im);

	im = loadImage(arg[1]);
	if (!im) goto error;
	gdImageFlipVertical(im);
	if (!savePngImage(im, "flip_vertical.png")) {
		goto error;
	}
	gdImageDestroy(im);

	im = loadImage(arg[1]);
	if (!im) goto error;
	gdImageFlipBoth(im);
	if (!savePngImage(im, "flip_both.png")) {
		goto error;
	}
	gdImageDestroy(im);

	goto done;

error:
	returncode = 1;

done:
	return returncode;
}
