/* test case for outside of buffer reads/write bug in _gdImageFillTiled */
#include "gd.h"
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im, tile;
	int tile_red, tile_blue;
	int error = 0;

	im = gdImageCreate(200, 150);

	tile = gdImageCreateTrueColor(2, 2);

	tile_red = gdImageColorAllocate(tile, 255, 0, 0);
	tile_blue = gdImageColorAllocate(tile, 0, 0, 255);

	gdImageSetPixel(tile, 0, 0, tile_red);
	gdImageSetPixel(tile, 1, 1, tile_red);
	gdImageSetPixel(tile, 1, 0, tile_blue);
	gdImageSetPixel(tile, 0, 1, tile_blue);

	gdImageSetTile(im, tile);
	gdImageFill(im, 11, 12, gdTiled);

	if (!gdAssertImageEqualsToFile("gdimagefill/bug00104_1_exp.png", im)) {
		error = 1;
	}

	gdImageDestroy(im);
	gdImageDestroy(tile);
	return error;
}
