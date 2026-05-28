/**
 * TrueVision TGA conformance suite read tests.
 */

#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

static void assert_conformance_file(const char *basename);

int main()
{
	assert_conformance_file("b5-attrib");
	assert_conformance_file("b5-cmap");
	assert_conformance_file("b5-noattrib");
	assert_conformance_file("b5-unused-cmap");
	assert_conformance_file("bottom_left");
	assert_conformance_file("bottom_right");
	assert_conformance_file("cbw8");
	assert_conformance_file("ccm8");
	assert_conformance_file("cmap_offset");
	assert_conformance_file("ctc24");
	assert_conformance_file("top_left");
	assert_conformance_file("top_right");
	assert_conformance_file("ubw8");
	assert_conformance_file("ucm8");
	assert_conformance_file("utc16");
	assert_conformance_file("utc24");
	assert_conformance_file("utc32");

	return gdNumFailures();
}

static void assert_conformance_file(const char *basename)
{
	char filename[64];
	gdImagePtr im;
	FILE *fp;
	char *expected;

	snprintf(filename, sizeof(filename), "%s.tga", basename);
	fp = gdTestFileOpenX("tga", "conformance", filename, NULL);
	gdTestAssertMsg(fp != NULL, "can't open %s", filename);
	if (fp == NULL) {
		return;
	}

	im = gdImageCreateFromTga(fp);
	fclose(fp);
	gdTestAssertMsg(im != NULL, "can't read %s", filename);
	if (im == NULL) {
		return;
	}

	snprintf(filename, sizeof(filename), "%s.png", basename);
	expected = gdTestFilePathX("tga", "conformance", filename, NULL);
	if (!gdTestImageCompareToFile(__FILE__, __LINE__, NULL, expected, im)) {
		gdTestErrorMsg("%s failed\n", basename);
	}
	gdFree(expected);

	gdImageDestroy(im);
}
