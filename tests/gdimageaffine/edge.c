#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst = NULL;
	double affine[6];
	int x, y;
	int partial_alpha_pixels = 0;

	src = gdImageCreateTrueColor(16, 16);
	if (!gdTestAssert(src != NULL)) {
		return 1;
	}

	gdImageAlphaBlending(src, 0);
	gdImageSaveAlpha(src, 1);
	gdImageSetInterpolationMethod(src, GD_LINEAR);
	gdImageFilledRectangle(src, 0, 0, 15, 15, gdTrueColorAlpha(255, 255, 255, gdAlphaOpaque));

	gdAffineRotate(affine, 10.0);
	if (!gdTestAssert(gdTransformAffineGetImage(&dst, src, NULL, affine) == GD_TRUE)) {
		gdImageDestroy(src);
		return gdNumFailures();
	}
	if (!gdTestAssert(dst != NULL)) {
		gdImageDestroy(src);
		return gdNumFailures();
	}

	for (y = 0; y < gdImageSY(dst); y++) {
		for (x = 0; x < gdImageSX(dst); x++) {
			const int alpha = gdTrueColorGetAlpha(gdImageGetTrueColorPixel(dst, x, y));
			if (alpha > gdAlphaOpaque && alpha < gdAlphaTransparent) {
				partial_alpha_pixels++;
			}
		}
	}

	gdTestAssertMsg(partial_alpha_pixels > 0,
	                "expected affine edge interpolation to create partial-alpha pixels");

	gdImageDestroy(dst);
	gdImageDestroy(src);
	return gdNumFailures();
}
