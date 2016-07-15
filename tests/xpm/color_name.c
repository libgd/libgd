#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int
main(void)
{
	gdImagePtr im;
	char *path;
	int c, result;

	path = gdTestFilePath("xpm/color_name.xpm");
	im = gdImageCreateFromXpm(path);
	free(path);
	if (!im) {
		return 2;
	}
	c = gdImageGetPixel(im, 2, 2);
	if (gdImageRed(im, c)      == 0xFF
	        && gdImageGreen(im, c) == 0xFF
	        && gdImageBlue(im, c)  == 0x0) {
		result = 0;
	} else {
		result = 1;
	}
	gdImageDestroy(im);
	return result;
}
