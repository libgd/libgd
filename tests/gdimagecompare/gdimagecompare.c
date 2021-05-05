/**
 * Base test for gdImageCompare()
 */

#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im1;
	gdImagePtr im2;
	int black;
	int red1, red2;
	int yellow1, yellow2;
	int white;
	int blue;
	int ret;

	im1 = gdImageCreate(128, 128);
	im2 = gdImageCreateTrueColor(256, 256);

	/* Set different interlace */
	gdImageInterlace(im1, 1);
	gdImageInterlace(im2, 2);

	/* Set different transparent */
	gdImageColorTransparent(im1, 10);
	gdImageColorTransparent(im2, 20);

	/* Allocate different color number */
	black = gdImageColorAllocate(im1, 0, 0, 0);
	red1 = gdImageColorAllocate(im1, 255, 0, 0);
	yellow1 = gdImageColorAllocate(im1, 255, 255, 0);
	red2 = gdImageColorAllocate(im2, 255, 0, 0);
	yellow2 = gdImageColorAllocate(im2, 255, 255, 0);
	white = gdImageColorAllocate(im2, 255, 255, 255);
	blue = gdImageColorAllocate(im2, 0, 0, 255);

	/* Filled different color */
	gdImageFilledRectangle(im1, 0, 0, 127, 8, black);
	gdImageFilledRectangle(im1, 9, 0, 127, 16, red1);
	gdImageFilledRectangle(im1, 17, 0, 127, 24, yellow1);
	gdImageFilledRectangle(im2, 0, 0, 127, 8, red2);
	gdImageFilledRectangle(im2, 9, 0, 127, 16, yellow2);
	gdImageFilledRectangle(im2, 17, 0, 127, 24, white);
	gdImageFilledRectangle(im2, 25, 0, 127, 32, blue);

	ret = gdImageCompare(im1, im2);

	gdTestAssert(ret == (GD_CMP_INTERLACE | GD_CMP_TRANSPARENT | GD_CMP_TRUECOLOR | GD_CMP_SIZE_X | GD_CMP_SIZE_Y | GD_CMP_COLOR | GD_CMP_IMAGE | GD_CMP_NUM_COLORS));

	gdImageDestroy(im1);
	gdImageDestroy(im2);

	return gdNumFailures();
}
