#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int c;

	im = gdImageCreate(1, 1);
	/* test for deallocating a color */
	c = gdImageColorAllocate(im, 255, 255, 255);
	if (c < 0) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageColorDeallocate(im, c);
	if (!im->open[c]) {
		gdImageDestroy(im);
		return 1;
	}

	/* just see whether it is OK with out-of-bounds values */
	gdImageColorDeallocate(im, gdMaxColors);
	gdImageColorDeallocate(im, -1);
	gdImageDestroy(im);

	return 0;
}
