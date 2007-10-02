#include <gd.h>
#include <stdio.h>
#include "gdtest.h"

#define exp_img "bug00032_exp.png"

int main()
{
	gdImagePtr im, tile;
	char exp[] = "bug00032_exp.png";
	FILE *fp;

	tile = gdImageCreateTrueColor(10, 10);
	gdImageFill(tile, 0, 0, 0xFFFFFF);
	gdImageLine(tile, 0,0, 9,9, 0xff0000);
	gdImageColorTransparent(tile, 0xFFFFFF);

	im = gdImageCreateTrueColor(50, 50);
	gdImageFilledRectangle(im, 0, 0, 25, 25, 0x00FF00);

	gdImageSetTile(im, tile);
	gdImageFilledRectangle(im, 10, 10, 49, 49, gdTiled);

	gdAssertImageEqualsToFile(exp_img, im);

 	/* Destroy it */
 	gdImageDestroy(im);
 	gdImageDestroy(tile);
	return 0;
}

