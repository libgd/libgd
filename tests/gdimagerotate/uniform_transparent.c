#include "gd.h"
#include "gdtest.h"

int main(void) {
	gdImagePtr src, dst;
	int transparent;
	int x, y;
	int changed = 0;

	src = gdImageCreateTrueColor(200, 200);
	gdTestAssert(src != NULL);
	if (src == NULL) {
		return gdNumFailures();
	}

	transparent = gdTrueColorAlpha(0, 0, 0, gdAlphaTransparent);
	gdImageAlphaBlending(src, 0);
	gdImageFilledRectangle(src, 0, 0, 199, 199, transparent);
	dst = gdImageRotateInterpolated(src, 45.0f, transparent);
	gdTestAssert(dst != NULL);
	if (dst != NULL) {
		for (y = 0; y < gdImageSY(dst); y++) {
			for (x = 0; x < gdImageSX(dst); x++) {
				if (gdImageGetTrueColorPixel(dst, x, y) != transparent) {
					changed++;
				}
			}
		}
		gdTestAssertMsg(changed == 0,
						"%d uniform transparent pixels changed", changed);
		gdImageDestroy(dst);
	}
	gdImageDestroy(src);

	return gdNumFailures();
}
