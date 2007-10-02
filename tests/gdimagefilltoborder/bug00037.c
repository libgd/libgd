#include <gd.h>
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int bordercolor, color;

	im = gdImageCreateTrueColor(100, 100);

	gdImageAlphaBlending(im, 1);
	gdImageSaveAlpha(im, 1);
	bordercolor = gdImageColorAllocateAlpha(im, 0, 0, 0, 2);
	color = gdImageColorAllocateAlpha(im, 0, 0, 0, 1);

	gdImageFillToBorder(im, 5, 5, bordercolor, color);

	color = gdImageGetPixel(im, 5, 5);

 	gdImageDestroy(im);
	if (gdTestAssert(color==0x1000000)) {
		return 0;
	} else {
		printf("c: %X, expected %X\n", color, 0x1000000);
		return -1;
	}

}

