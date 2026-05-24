#include "gd.h"
#include "gdtest.h"

/* 
 using gradient here so it can be easily verified so repeated row scaling can be caught easily
*/
static gdImagePtr
make_vertical_gradient(int width, int height)
{
	gdImagePtr im = gdImageCreateTrueColor(width, height);
	int x, y;
	int red;

	gdTestAssert(im != NULL);

	for (y = 0; y < height; y++) {
		red = y * 60;
		for (x = 0; x < width; x++) {
			gdImageSetPixel(im, x, y, gdTrueColorAlpha(red, x * 30, 0, 0));
		}
	}

	gdImageSetInterpolationMethod(im, GD_BICUBIC_FIXED);

	return im;
}

int main()
{
	gdImagePtr src = make_vertical_gradient(4, 4);
	gdImagePtr dst;
	int top;
	int between;

	dst = gdImageScale(src, 4, 8);
	gdTestAssert(dst != NULL);
	gdTestAssert(dst->sx == 4);
	gdTestAssert(dst->sy == 8);

	top = gdTrueColorGetRed(gdImageGetTrueColorPixel(dst, 0, 0));
	between = gdTrueColorGetRed(gdImageGetTrueColorPixel(dst, 0, 1));
	gdTestAssertMsg(between > top,
		"bicubic fixed scaling should interpolate between source rows");

	gdImageDestroy(dst);
	gdImageDestroy(src);

	return gdNumFailures();
}
