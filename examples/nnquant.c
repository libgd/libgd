/* $Id$ */

#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

void save_png(gdImagePtr im, const char *filename)
{
	FILE *fp;
	fp = fopen(filename, "wb");
	if (!fp) {
		fprintf(stderr, "Can't save png image %s\n", filename);
		return;
	}
#ifdef HAVE_LIBPNG
	gdImagePng(im, fp);
#else
	printf("No PNG support. Cannot save image.\n");
#endif
	fclose(fp);
}

int main()
{
#ifdef HAVE_JPEG
	gdImagePtr im, im2;
	FILE *fp;
	char path[2048];

	fp=fopen("resampledbug.jpeg", "rb");
	if (!fp) {
		fprintf(stderr, "Can't load /home/pierre/IM3801.jpg\n");
		return 1;
	}

	im = gdImageCreateFromJpeg(fp);
	fclose(fp);
	if (!im) {
		fprintf(stderr, "Can't load TIFF image %s\n", path);
		return 1;
	}

	im2 = gdImageNeuQuant(im, 256, 3);

	if (im2) {
		gdImageSaveAlpha(im2, 1);
		save_png(im2, "a_nnquant.png");
		gdImageDestroy(im2);
	} else {
		printf("neu quant failed.\n");
	}

	gdImageTrueColorToPalette(im, 1, 256);

	gdImageSaveAlpha(im, 1);
	save_png(im, "a_jquant_dither.png");

	gdImageDestroy(im);
#else
	printf("JPEG support is required for this example. Please recompile GD with JPEG or change this example to use another format as input.");
	return 1;
#endif
	return 0;
}
