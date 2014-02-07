#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int x, y, c;
	int r=0;


	im = gdImageCreateTrueColor(256, 256);
	gdImageAlphaBlending( im, gdEffectReplace );
	for (x=0; x<256; x++) {
		for (y=0; y<256; y++) {
			c = (y/2 << 24) + (x << 16) + (x << 8) + x;
			gdImageSetPixel(im, x, y, c );
		}
	}
	gdImageAlphaBlending( im, gdEffectMultiply );
	gdImageFilledRectangle(im, 0, 0, 255, 255, 0xff7f00);

	if (gdTrueColorGetGreen(gdImageGetPixel(im, 0, 128)) != 0x40) {
		r = 1;
	}
	if (gdTrueColorGetGreen(gdImageGetPixel(im, 128, 128)) != 0x5f) {
		r = 1;
	}
	if (gdTrueColorGetGreen(gdImageGetPixel(im, 255, 128)) != 0x7f) {
		r = 1;
	}
	gdImageDestroy(im);
	return r;
}
