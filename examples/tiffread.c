/* $Id$ */
#include <gd.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
 	gdImagePtr im;
	FILE *fp;
	char path[2048];

//	sprintf(path, "/home/pierre/projects/gd/patches/tiff/images/cramps-tile.tif");
	sprintf(path, "/home/pierre/projects/gd/patches/tiff/images/ycbcr-cat.tif");

	printf("opening %s\n", path);
	fp = fopen(path, "rb");
	if (!fp) {
		printf("failed, cannot open file\n");
		return 1;
	}

	im = gdImageCreateFromTiff(fp);
	fclose(fp);
	if (!im) {
		fprintf(stderr, "Can't load TIFF image %s\n", path);
		return 1;
	}

	fp = fopen("fromtiff.png", "wb");
	if (!fp) {
		fprintf(stderr, "Can't save png image fromtiff.png\n");
		gdImageDestroy(im);
		return 1;
	}

	gdImagePng(im, fp);
	fclose(fp);
	gdImageDestroy(im);
}
