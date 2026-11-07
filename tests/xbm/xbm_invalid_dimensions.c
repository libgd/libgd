/*
	Test that XBM dimensions outside 1..INT_MAX are rejected, and that a
	width/height pair whose byte count overflows int is rejected, instead of
	being silently wrapped into a different image size.
*/

#include "gd.h"
#include "gdtest.h"
#include <stdlib.h>

static gdImagePtr readXbm(const char *width, const char *height)
{
	FILE *fp;
	gdImagePtr im;
	char *file;

	file = gdTestTempFile("xbm_invalid_dimensions.xbm");
	fp = fopen(file, "w+b");
	free(file);
	if (!fp) {
		gdTestErrorMsg("cannot create temp file\n");
		return NULL;
	}
	fprintf(fp, "#define x_width %s\n#define x_height %s\n"
			"static unsigned char x_bits[] = {\n"
			"   0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};\n",
			width, height);
	im = gdImageCreateFromXbm(fp);
	fclose(fp);
	return im;
}

static void assertRejected(const char *width, const char *height)
{
	gdImagePtr im = readXbm(width, height);

	if (!gdTestAssertMsg(im == NULL, "%s x %s accepted as %d x %d\n",
						 width, height, im ? gdImageSX(im) : 0,
						 im ? gdImageSY(im) : 0)) {
		gdImageDestroy(im);
	}
}

int main()
{
	gdImagePtr im;

	assertRejected("4294967298", "2");
	assertRejected("2", "4294967298");
	assertRejected("-8", "2");
	assertRejected("2", "-8");
	assertRejected("0", "2");
	assertRejected("2147483647", "9");

	im = readXbm("8", "4");
	if (gdTestAssertMsg(im != NULL, "8 x 4 rejected\n")) {
		gdTestAssert(gdImageSX(im) == 8);
		gdTestAssert(gdImageSY(im) == 4);
		gdImageDestroy(im);
	}

	return gdNumFailures();
}
