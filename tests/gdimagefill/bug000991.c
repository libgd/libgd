#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im = NULL;
	gdImagePtr tile = NULL;
	const int x = 100000;
	const int y = 0;

	im = gdImageCreateTrueColor(8, 8);
	if (!gdTestAssert(im != NULL)) {
		return 1;
	}

	tile = gdImageCreateTrueColor(2, 2);
	if (!gdTestAssert(tile != NULL)) {
		gdImageDestroy(im);
		return 1;
	}

	/* Initialize tile with valid content and attach it as tiled fill source. */
	gdImageFill(tile, 0, 0, 0x00ffffff);
	gdImageSetTile(im, tile);

	/* Regression scenario: out-of-bounds X with tiled fill must not crash. */
	gdImageFill(im, x, y, gdTiled);

	gdImageDestroy(tile);
	gdImageDestroy(im);

	return gdNumFailures();
}
