#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	gdImagePtr tile;
	int red, green, blue, other;
	int i, r = 0;

	im = gdImageCreateTrueColor(100, 100);
	tile = gdImageCreate(10, 10);
	red   = gdImageColorAllocate(tile, 0xFF, 0, 0);
	green = gdImageColorAllocate(tile, 0, 0xFF, 0);
	blue  = gdImageColorAllocate(tile, 0, 0, 0xFF);
	other = gdImageColorAllocate(tile, 0, 0, 0x2);
	gdImageFilledRectangle(tile, 0, 0, 2, 10, red);
	gdImageFilledRectangle(tile, 3, 0, 4, 10, green);
	gdImageFilledRectangle(tile, 5, 0, 7, 10, blue);
	gdImageFilledRectangle(tile, 8, 0, 9, 10, other);
	gdImageColorTransparent(tile, blue);
	gdImageSetTile(im, tile);
	for (i=0; i<100; i++) {
		gdImageSetPixel(im, i, i, gdTiled);
	}
	if (gdTrueColorGetBlue(gdImageGetPixel(im, 9, 9)) != 0x2) {
		r = 1;
	}
	gdImageDestroy(tile);
	gdImageDestroy(im);
	return r;
}
