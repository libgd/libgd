#include <stdio.h>

#include "gd.h"
#include "gdtest.h"

static void assert_conformance_file(const char *basename);

int main()
{
	assert_conformance_file("dice");
	assert_conformance_file("edgecase");
	assert_conformance_file("kodim10");
	assert_conformance_file("kodim23");
	assert_conformance_file("qoi_logo");
	assert_conformance_file("testcard");
	assert_conformance_file("testcard_rgba");
	assert_conformance_file("wikipedia_008");

	return gdNumFailures();
}

static void assert_conformance_file(const char *basename)
{
	char filename[64];
	FILE *fp;
	gdImagePtr im;
	char *expected;

	snprintf(filename, sizeof(filename), "%s.qoi", basename);
	fp = gdTestFileOpenX("qoi", "conformance", filename, NULL);
	gdTestAssertMsg(fp != NULL, "can't open %s", filename);
	if (fp == NULL) {
		return;
	}

	im = gdImageCreateFromQoi(fp);
	fclose(fp);
	gdTestAssertMsg(im != NULL, "can't read %s", filename);
	if (im == NULL) {
		return;
	}

	snprintf(filename, sizeof(filename), "%s.png", basename);
	expected = gdTestFilePathX("qoi", "conformance", filename, NULL);
	if (!gdTestImagePerceptualCompareToFile(__FILE__, __LINE__, NULL, expected, im, 0.0, 0)) {
		gdTestErrorMsg("%s failed\n", basename);
	}
	gdFree(expected);
	gdImageDestroy(im);
}
