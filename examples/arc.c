#include "gd.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	gdImagePtr im;
	FILE *fp;
	int cor_rad = 60;
	im = gdImageCreateTrueColor(400, 400);
	gdImageFilledRectangle(im, 0, 0, 399, 399, 0x00FFFFFF);

	gdImageFilledArc (im, cor_rad, 399 - cor_rad, cor_rad *2, cor_rad *2, 90, 180, 0x0, gdPie);

	fp = fopen("b.png", "wb");
	if (!fp) {
		fprintf(stderr, "Can't save png image.\n");
		gdImageDestroy(im);
		return 1;
	}
	gdImagePng(im, fp);
	fclose(fp);

	gdImageDestroy(im);
	return 0;
}
