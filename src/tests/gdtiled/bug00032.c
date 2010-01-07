#include <gd.h>
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im, tile;
	char exp[] = "bug00032_exp.png";
	FILE *fp;
	const char *file_exp = "bug00132_exp.png";
	char path[2048];

	tile = gdImageCreateTrueColor(10, 10);
	if (!im) {
		fprintf(stderr, "Can't create a 10x10 true color image.");
		return -1;
	}

	gdImageFill(tile, 0, 0, 0xFFFFFF);
	gdImageLine(tile, 0,0, 9,9, 0xff0000);
	gdImageColorTransparent(tile, 0xFFFFFF);

	im = gdImageCreateTrueColor(50, 50);
	if (!im) {
		fprintf(stderr, "Can't create a 50x50 true color image.");
		gdImageDestroy(tile);
		return -1;
	}
	gdImageFilledRectangle(im, 0, 0, 25, 25, 0x00FF00);

	gdImageSetTile(im, tile);
	gdImageFilledRectangle(im, 10, 10, 49, 49, gdTiled);

	sprintf(path, "%s/gdtiled/%s", GDTEST_TOP_DIR, file_exp);
	gdAssertImageEqualsToFile(path, im);

 	/* Destroy it */
 	gdImageDestroy(im);
 	gdImageDestroy(tile);
	return 0;
}

