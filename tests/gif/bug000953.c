#include <gd.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	void *gif;
	int size = 0;

	im = gdImageCreateTrueColor(5, 4);
	if (!gdTestAssert(im != NULL)) {
		return gdNumFailures();
	}

	gdImageFilledRectangle(im, 0, 0, 4, 3, gdTrueColor(255, 255, 255));
	gdImageLine(im, 0, 0, 4, 3, gdTrueColor(0, 0, 0));
	gdImageLine(im, 4, 0, 0, 3, gdTrueColor(255, 0, 0));

	gdTestAssert(gdImageTrueColorToPaletteSetMethod(im, GD_QUANT_NEUQUANT, 0) == GD_TRUE);

	gif = gdImageGifPtr(im, &size);
	gdTestAssertMsg(gif != NULL, "expected GIF output");
	gdTestAssertMsg(size > 0, "expected nonempty GIF output, got size %d", size);
	gdTestAssertMsg(im->trueColor, "gdImageGifPtr must not convert source image in-place");

	if (gif != NULL) {
		gdFree(gif);
	}
	gdImageDestroy(im);

	return gdNumFailures();
}
