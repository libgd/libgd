#include "gd.h"
#include "gdtest.h"

int main()
{
 	gdImagePtr im;
	int c1,c2,c3,c4,c5;

	im = gdImageCreateTrueColor(100,100);
	gdImageFilledRectangle(im, 2,2, 80,95, 0x50FFFFFF);

	c1 = gdImageGetTrueColorPixel(im, 2, 2);
	c2 = gdImageGetTrueColorPixel(im, 80, 95);
	c3 = gdImageGetTrueColorPixel(im, 80, 2);
	c4 = gdImageGetTrueColorPixel(im, 2, 95);
	c5 = gdImageGetTrueColorPixel(im, 49, 49);

	gdImageDestroy(im);
	if (0x005e5e5e == c1 && 0x005e5e5e == c2 &&
	 0x005e5e5e == c3 && 0x005e5e5e == c4) {
	 	return 0;
	} else {
		return 1;
	}
}

