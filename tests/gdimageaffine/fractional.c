#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr src, dst = NULL;
	gdRect area;
	double affine[6] = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
	int px;

	src = gdImageCreateTrueColor(2, 2);
	if (!gdTestAssert(src != NULL)) {
		return 1;
	}

	gdImageSetInterpolationMethod(src, GD_LINEAR);
	src->tpixels[0][0] = gdTrueColorAlpha(0, 0, 0, gdAlphaOpaque);
	src->tpixels[0][1] = gdTrueColorAlpha(255, 0, 0, gdAlphaOpaque);
	src->tpixels[1][0] = gdTrueColorAlpha(0, 255, 0, gdAlphaOpaque);
	src->tpixels[1][1] = gdTrueColorAlpha(0, 0, 255, gdAlphaOpaque);

	area.x = 0;
	area.y = 0;
	area.width = 2;
	area.height = 2;

	if (!gdTestAssert(gdTransformAffineGetImage(&dst, src, &area, affine) == GD_TRUE)) {
		gdImageDestroy(src);
		return gdNumFailures();
	}
	if (!gdTestAssert(dst != NULL)) {
		gdImageDestroy(src);
		return gdNumFailures();
	}

	px = gdImageGetTrueColorPixel(dst, 0, 0);
	gdTestAssertMsg(gdTrueColorGetRed(px) > 0,
	                "expected fractional affine sample to include red, got %x", px);
	gdTestAssertMsg(gdTrueColorGetGreen(px) > 0,
	                "expected fractional affine sample to include green, got %x", px);
	gdTestAssertMsg(gdTrueColorGetBlue(px) > 0,
	                "expected fractional affine sample to include blue, got %x", px);

	gdImageDestroy(dst);
	gdImageDestroy(src);
	return gdNumFailures();
}
