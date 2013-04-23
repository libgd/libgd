#include "gd.h"
#include <stdio.h>
#include "gdtest.h"

#define exp_img "bug00032_exp.png"

#ifdef _MSC_VER
# define snprintf _snprintf
#endif

int main()
{
	gdImagePtr im, tile;
	char path[GDTEST_STRING_MAX];

	gdSetErrorMethod(gdSilence);

	snprintf(path, GDTEST_STRING_MAX - 1, "%s/gdtiled/%s", GDTEST_TOP_DIR, exp_img);

	tile = gdImageCreateTrueColor(10, 10);
	gdImageFill(tile, 0, 0, 0xFFFFFF);
	gdImageLine(tile, 0,0, 9,9, 0xff0000);
	gdImageColorTransparent(tile, 0xFFFFFF);

	im = gdImageCreateTrueColor(50, 50);
	gdImageFilledRectangle(im, 0, 0, 25, 25, 0x00FF00);

	gdImageSetTile(im, tile);
	gdImageFilledRectangle(im, 10, 10, 49, 49, gdTiled);

	if (!gdAssertImageEqualsToFile(path, im)) {
		gdImageDestroy(im);
		gdImageDestroy(tile);
		return 1;
	}

	gdImageDestroy(im);
	gdImageDestroy(tile);
	return 0;
}

