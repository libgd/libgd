#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int c1,c2,c3,c4;

	im = gdImageCreateTrueColor(10,10);
	if (!im) {
		return 1;
	}

	gdImageFilledRectangle(im, 1,1, 1,1, 0x50FFFFFF);
	c1 = gdImageGetTrueColorPixel(im, 1, 1);
	c2 = gdImageGetTrueColorPixel(im, 2, 1);
	c3 = gdImageGetTrueColorPixel(im, 1, 2);
	c4 = gdImageGetTrueColorPixel(im, 2, 2);
	gdImageDestroy(im);
	if (0x005e5e5e == c1 && 0x0 == c2 &&
	        0x0 == c3 && 0x0 == c4) {
		return 0;
	} else {
		return 1;
	}
}
