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
	double angle, a2;

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
		+ 10 pixels margin
	 */

	/* to radian */
	a2 = angle * .0174532925;

	new_width = fabs(ceil(cos(a2) * gdImageSX(im))) +
	            fabs(sin(a2) * gdImageSY(im));
	new_height = fabs(ceil(cos(a2) * gdImageSY(im))) +
	             fabs(sin(a2) * gdImageSX(im));


	im2 = gdImageCreateTrueColor(new_width, new_height);
	if (!im2) {
		fprintf(stderr, "Can't create a new image");
		gdImageDestroy(im);
		return 1;
	}

	gdImageAlphaBlending(im2, 0);
	gdImageFilledRectangle(im2, 0, 0, gdImageSX(im2), gdImageSY(im2), gdTrueColorAlpha(127,0,0,127));

	gdImageCopyRotated(im2, im, new_width/2, new_height/2, 0, 0, gdImageSX(im), gdImageSY(im), angle);
	gdImageSaveAlpha(im2, 1);
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
