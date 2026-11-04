/* Test that gdImageCopyResampled() mirrors the image for negative source sizes
 * See https://github.com/php/php-src/issues/25175 */

#include "gd.h"
#include "gdtest.h"

#define RED 0xff0000
#define BLUE 0x0000ff

static void check_flip(gdImagePtr src, int srcX, int srcY, int srcW, int srcH,
					   int c00, int c10, int c01, int c11) {
	gdImagePtr dst = gdImageCreateTrueColor(100, 100);

	gdImageCopyResampled(dst, src, 0, 0, srcX, srcY, 100, 100, srcW, srcH);
	gdTestAssert(gdImageGetPixel(dst, 0, 0) == c00);
	gdTestAssert(gdImageGetPixel(dst, 99, 0) == c10);
	gdTestAssert(gdImageGetPixel(dst, 0, 99) == c01);
	gdTestAssert(gdImageGetPixel(dst, 99, 99) == c11);
	gdImageDestroy(dst);
}

int main() {
	gdImagePtr src, dst;
	int expected[] = {0x400000, 0x400000, 0x300000, 0x300000,
					  0x200000, 0x200000, 0x100000, 0x100000};
	int x;

	src = gdImageCreateTrueColor(100, 100);
	gdImageFilledRectangle(src, 0, 0, 99, 99, BLUE);
	gdImageFilledRectangle(src, 0, 0, 9, 9, RED);

	check_flip(src, 99, 0, -100, 100, BLUE, RED, BLUE, BLUE);
	check_flip(src, 0, 99, 100, -100, BLUE, BLUE, RED, BLUE);
	check_flip(src, 99, 99, -100, -100, BLUE, BLUE, BLUE, RED);
	gdImageDestroy(src);

	/* scaled mirror */
	src = gdImageCreateTrueColor(4, 1);
	for (x = 0; x < 4; x++) {
		gdImageSetPixel(src, x, 0, (x + 1) * 0x100000);
	}
	dst = gdImageCreateTrueColor(8, 1);
	gdImageCopyResampled(dst, src, 0, 0, 3, 0, 8, 1, -4, 1);
	for (x = 0; x < 8; x++) {
		gdTestAssertMsg(gdImageGetPixel(dst, x, 0) == expected[x],
						"pixel %d: expected %06x, got %06x\n", x, expected[x],
						gdImageGetPixel(dst, x, 0));
	}
	gdImageDestroy(dst);
	gdImageDestroy(src);

	return gdNumFailures();
}
