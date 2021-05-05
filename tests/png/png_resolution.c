#include "gd.h"
#include <stdio.h>
#include <stdlib.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	void *data;
	int size, red;

	im = gdImageCreate(100, 100);
	gdImageSetResolution(im, 72, 300);
	red = gdImageColorAllocate(im, 0xFF, 0x00, 0x00);
	gdImageFilledRectangle(im, 0, 0, 99, 99, red);
	data = gdImagePngPtr(im, &size);
	gdImageDestroy(im);

	im = gdImageCreateFromPngPtr(size, data);
	gdFree(data);

	if (!gdTestAssert(gdImageResolutionX(im) == 72) || !gdTestAssert(gdImageResolutionY(im) == 300)) {
		gdTestErrorMsg("failed image resolution X (%d != 72) or Y (%d != 300)\n", gdImageResolutionX(im), gdImageResolutionY(im));
		gdImageDestroy(im);
		return 1;
	}

	gdImageDestroy(im);
	return 0;
}
