#include "gd.h"
#include "gdtest.h"

gdImagePtr createTransparentStripedImage(int size)
{
	gdImagePtr im;
	int white, black;

	im = gdImageCreate(size, size);
	white = gdImageColorAllocate(im, 255, 255, 255);
	black = gdImageColorAllocate(im, 0, 0, 0);
	gdImageFilledRectangle(im, 0, 0, size, size / 2, white);
	gdImageFilledRectangle(im, 0, size / 2, size, size, black);
	gdImageColorTransparent(im, white);

	return im;
}

int main()
{
	int error = 0;
	gdImagePtr src;
	gdImagePtr dst;
	gdImagePtr exp;

	src = createTransparentStripedImage(64);
	dst = gdImageCreate(32, 32);

	gdImageCopyResized(dst, src, 0, 0, 0, 0, 32, 32, 64, 64);

	exp = createTransparentStripedImage(32);

	if (!gdAssertImageEquals(exp, dst)) {
		gdTestErrorMsg("image differs from expected result");
		error = 1;
	}

	gdImageDestroy(src);
	gdImageDestroy(dst);
	gdImageDestroy(exp);

	return error;
}
