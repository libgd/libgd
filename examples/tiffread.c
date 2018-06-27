/*
 * You can fetch a set of samples TIFF images here:
 * ftp://ftp.remotesensing.org/pub/libtiff/
 * (pics-x.y.z.tar.gz)
 */

#include <gd.h>
#include <stdio.h>
#include <stdlib.h>
int main()
{
	gdImagePtr im;
	FILE *fp;
	char path[9][2048];
	int i;
	char dst[2048];

	sprintf(path[0], "cramps-tile.tif");
	sprintf(path[1], "cramps.tif");
	sprintf(path[2], "ycbcr-cat.tif");
	sprintf(path[3], "jello.tif");
	sprintf(path[4], "caspian.tif");
	sprintf(path[5], "strike.tif");
	sprintf(path[6], "off_luv24.tif");
	sprintf(path[7], "off_l16.tif");
	sprintf(path[8], "fax2d.tif");

	for (i = 0; i < 9; i++) {
		printf("opening %s\n", path[i]);
		fp = fopen(path[i], "rb");
		if (!fp) {
			printf("failed, cannot open file\n");
			return 1;
		}

		im = gdImageCreateFromTiff(fp);
		fclose(fp);
		if (!im) {
			fprintf(stderr, "Can't load TIFF image %s\n", path[i]);
			return 1;
		}


		sprintf(dst, "%i.png", i);

		fp = fopen(dst, "wb");
		if (!fp) {
			fprintf(stderr, "Can't save png image fromtiff.png\n");
			gdImageDestroy(im);
			return 1;
		}

		gdImagePng(im, fp);
		fclose(fp);
		gdImageDestroy(im);
	}
	return 0;
}
