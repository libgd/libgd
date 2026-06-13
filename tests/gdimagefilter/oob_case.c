#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im, blurred;

	im = gdImageCreateTrueColor(100, 100);
	if (!gdTestAssert(im != NULL)) {
		return 1;
	}

	blurred = gdImageCopyGaussianBlurred(im, 100, -1.0);
	gdTestAssertMsg(blurred != NULL,
	                "expected radius equal to image dimensions to be clamped");
	if (blurred != NULL) {
		gdTestAssertMsg(gdImageSX(blurred) == gdImageSX(im),
		                "expected output width %d, got %d",
		                gdImageSX(im), gdImageSX(blurred));
		gdTestAssertMsg(gdImageSY(blurred) == gdImageSY(im),
		                "expected output height %d, got %d",
		                gdImageSY(im), gdImageSY(blurred));
		gdImageDestroy(blurred);
	}

	blurred = gdImageCopyGaussianBlurred(im, 0, -1.0);
	gdTestAssertMsg(blurred == NULL,
	                "expected radius 0 to be rejected");
	if (blurred != NULL) {
		gdImageDestroy(blurred);
	}

	gdImageDestroy(im);
	return gdNumFailures();
}
