#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im, im2;
	int error = 0;

	im = gdImageCreateTrueColor(100, 100);

	if (im == NULL) {
		gdTestErrorMsg("gdImageCreateTruecolor failed\n");
		error = 1;
		goto exit;
	}
	gdImageColorTransparent(im, -1);
	gdImageTrueColorToPalette(im, 1, 3);
	gdImageColorTransparent(im, 9);
	im2 = gdImageScale(im, 1, 65535);
	if (im2 == NULL) {
		error = 1;
		goto freeim;
	} else {
		gdImageDestroy(im2);
	}

freeim:
	gdImageDestroy(im);
exit:
	return error;
}
