#include <stdio.h>
#include "gd.h"

#include "gdtest.h"

int main() {
	gdImagePtr im;
	int black, white;
	int error = 0;
	int xs = 300, xe = 350, i;
	im = gdImageCreateTrueColor(400, 200);

	if (im == NULL) {
		gdTestErrorMsg("gdImageCreate failed.\n");
		return 1;
	}

	white = gdImageColorAllocate(im, 255,255,255);
	black = gdImageColorAllocate(im, 0,0,0);

	gdImageFilledRectangle(im, 0, 0, 400,200, white);

	gdImageRectangle(im, xs, 95, xe, 95, black);

	for (i = xs; i <= xe; i++) {
		int c = gdImageGetPixel(im, i, 94);
		if (c != white) {
			error |=1;
		}
	}
	for (i = xs; i <= xe; i++) {
		int c = gdImageGetPixel(im, i, 95);
		if (c != black) {
			error |=1;
		}
	}
	for (i = xs; i <= xe; i++) {
		int c = gdImageGetPixel(im, i, 96);
		if (c != white) {
			error |=1;
		}
	}
	gdImageDestroy(im);

	return error;
}
