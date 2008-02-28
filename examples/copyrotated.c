/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

gdImagePtr loadImage(const char *name)
{
	FILE *fp;
	gdImagePtr im;

	fp = fopen(name, "rb");
	if (!fp) {
		fprintf(stderr, "Can't open jpeg file\n");
		gdImageDestroy(im);
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
 	gdImagePtr im, im2;
 	int new_width, new_height;
 	double angle;

	if (argc < 3) {
		fprintf(stderr, "Usage: copyrotated [angle in degree] [filename.png]\n");
		return 1;
	}
	angle = strtod(arg[1], 0);
	im = loadImage(arg[2]);

	if (!im) {
		fprintf(stderr, "Can't load PNG file <%s>", arg[1]);
		return 1;
	}

	/*
		cos adj hyp (cos = adjacent / hypothenus)
		sin op hyp (sin adjacent / hypothenus)
	 */
	new_width = cos(angle * .0174532925) * gdImageSX(im)
				- sin(angle * .0174532925) * gdImageSY(im) + 1;

	new_height = - sin(angle * .0174532925) * gdImageSX(im)
		 		+ cos(angle * .0174532925) * gdImageSY(im) + 1;


	im2 = gdImageCreateTrueColor(new_width, new_height);
	if (!im2) {
		fprintf(stderr, "Can't create a new image");
		gdImageDestroy(im);
		return 1;
	}

	gdImageCopyRotated(im2, im, new_width/2, new_height/2, 0, 0, gdImageSX(im), gdImageSY(im), angle);
	if (!savePngImage(im2, "rotated.png")) {
		fprintf(stderr, "Can't save PNG file rotated.png");
		gdImageDestroy(im);
		gdImageDestroy(im2);
		return 1;
	}

	gdImageDestroy(im2);
	gdImageDestroy(im);
	return 0;
}
