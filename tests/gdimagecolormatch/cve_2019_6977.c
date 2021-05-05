/**
 * Test for CVE-2019-6977
 */

#include "gd.h"

int main()
{
	gdImagePtr im1;
	gdImagePtr im2;

	im1 = gdImageCreateTrueColor(0xfff, 0xfff);
	im2 = gdImageCreate(0xfff, 0xfff);
	if (gdImageColorAllocate(im2, 0, 0, 0) < 0)
	{
		gdImageDestroy(im1);
		gdImageDestroy(im2);
		return 1;
	}
	gdImageSetPixel(im2, 0, 0, 255);
	gdImageColorMatch(im1, im2);
	gdImageDestroy(im1);
	gdImageDestroy(im2);
	return 0;
}
