#include "gd.h"
#include "gdtest.h"

#include <stdlib.h>

static void test_unicode_charmap(void) {
	char *font = gdTestFilePath("freetype/Rochester-Regular.otf");
	gdImagePtr im = gdImageCreateTrueColor(100, 80);
	const int white = gdTrueColor(255, 255, 255);
	const int black = gdTrueColor(0, 0, 0);
	char *error;
	int changed = 0;
	int x, y;

	gdImageFilledRectangle(im, 0, 0, 99, 79, white);
	error = gdImageStringFT(im, NULL, black, font, 45, 0, 15, 60,
						  "\xC3\x9E\xC3\xB6");
	gdTestAssertMsg(error == NULL, "%s\n", error ? error : "");
	for (y = 0; y < 80; y++) {
		for (x = 0; x < 100; x++) {
			if (gdImageGetTrueColorPixel(im, x, y) != white) {
				changed++;
			}
		}
	}
	gdTestAssert(changed >= 10);
	gdImageDestroy(im);
	free(font);
}

static void test_macroman_compatibility(void) {
	char *font = gdTestFilePath("freetype/DejaVuSans.ttf");
	gdFTStringExtra extra = {0};
	int brect[8];
	char *error;

	extra.flags = gdFTEX_CHARMAP;
	extra.charmap = gdFTEX_MacRoman;
	error = gdImageStringFTEx(NULL, brect, 0, font, 12, 0, 0, 0, "Hello",
							 &extra);
	gdTestAssertMsg(error == NULL, "%s\n", error ? error : "");
	free(font);
}

int main(void) {
	test_unicode_charmap();
	test_macroman_compatibility();
	gdFontCacheShutdown();
	return gdNumFailures();
}
