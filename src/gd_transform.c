#include "gd.h"

BGD_DECLARE(void)
gdImageFlipHorizontal(gdImagePtr im)
{
	register int x, y;


	if (im->trueColor) {
		int p;
		for (y = 0; y < im->sy / 2; y++) {
			for (x = 0; x < im->sx; x++) {
				p = im->tpixels[y][x];
				im->tpixels[y][x] =	im->tpixels[im->sy - 1 - y][x];
				im->tpixels[im->sy - 1 - y][x] = p;
			}
		}
	} else {
		unsigned char p;
		for (y = 0; y < im->sy / 2; y++) {
			for (x = 0; x < im->sx; x++) {
				p = im->tpixels[y][x];
				im->tpixels[y][x] =	im->tpixels[im->sy - 1 - y][x];
				im->tpixels[im->sy - 1 - y][x] = p;
			}
		}

	}
	return;
}

BGD_DECLARE(void)
gdImageFlipVertical(gdImagePtr im)
{

	int x, y;

	if (im->trueColor) {
		int p;

		for (y = 0; y < im->sy; y++) {
			for (x = 0; x < im->sx / 2; x++) {
				p = im->tpixels[y][x];
				im->tpixels[y][x] =	im->tpixels[y][im->sx - 1 - x];
				im->tpixels[y][im->sx - 1 - x] = p;
			}
		}
	} else {
		unsigned char p;

		for (y = 0; y < im->sy; y++) {
			for (x = 0; x < im->sx / 2; x++) {
				p = im->pixels[y][x];
				im->pixels[y][x] =	im->pixels[y][im->sx - 1 - x];
				im->pixels[y][im->sx - 1 - x] = p;
			}
		}
	}
}

BGD_DECLARE(void)
gdImageFlipBoth(gdImagePtr im)
{
	gdImageFlipVertical(im);
	gdImageFlipHorizontal(im);
}

