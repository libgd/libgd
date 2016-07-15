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

	path = gdTestFilePath("xpm/bug00166.xpm");
	im = gdImageCreateFromXpm(path);
	free(path);
	if (!im) {
		return 2;
	}
	c = gdImageGetPixel(im, 1, 1);
	if (gdImageRed(im, c)      == 0xAA
	        && gdImageGreen(im, c) == 0xBB
	        && gdImageBlue(im, c)  == 0xCC) {
		result = 0;
	} else {
		result = 1;
	}
	gdImageDestroy(im);
	return result;
}
