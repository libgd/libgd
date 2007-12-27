#include "gd.h"

BGD_DECLARE(int)
gdImageFlipHorizontal(gdImagePtr im)
{
	int *p1, *p2, *src;
	register int x, y, tmp;


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
	return 1;
}

BGD_DECLARE(int)
gdImageFlipVertical(gdImagePtr im)
{

	int x, y;
	int w = im->sx;
	int h = im->sy;

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

