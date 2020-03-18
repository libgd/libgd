#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

int main(void) {
	char *path = gdTestFilePath("bmp/bug00450.bmp");
	gdImagePtr im = gdImageCreateFromFile(path);
	gdTestAssert(im != NULL);

	if (im) {
		char const ref[] =
			"#         "
			"##        "
			"###       "
			"####      "
			"#####     "
			"######    "
			"#######   "
			"########  "
			"######### "
			"##########"
			;
		int x, y;

		for (y = 0; y < 10; ++y) {
			for (x = 0; x < 10; ++x) {
				int const expected_pixel = ref[x + y * 10] == '#';
				int const actual_pixel = gdImageGetPixel(im, x, y);
				gdTestAssert(expected_pixel == actual_pixel);
			}
		}

		gdImageDestroy(im);
	}

	free(path);

	return gdNumFailures();
}
