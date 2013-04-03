/* $Id$ */
#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int
main(void)
{
	gdImagePtr im;
	char path[1024];
	int c, result;

	sprintf(path, "%s/xpm/color_name.xpm", GDTEST_TOP_DIR);
	im = gdImageCreateFromXpm(path);
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
