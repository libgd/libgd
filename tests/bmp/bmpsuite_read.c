#include "gd.h"
#include "gdtest.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	const char *bmp;
	const char *png;
} bmpsuite_good_t;

static const bmpsuite_good_t good_files[] = {
	{"pal1", "pal1"},
	{"pal1bg", "pal1bg"},
	{"pal1wb", "pal1"},
	{"pal4", "pal4"},
	{"pal4gs", "pal4gs"},
	{"pal4rle", "pal4"},
	{"pal8", "pal8"},
	{"pal8-0", "pal8"},
	{"pal8gs", "pal8gs"},
	{"pal8rle", "pal8"},
	{"pal8w126", "pal8w126"},
	{"pal8w125", "pal8w125"},
	{"pal8w124", "pal8w124"},
	{"pal8topdown", "pal8"},
	{"pal8nonsquare", "pal8nonsquare-e"},
	{"pal8os2", "pal8"},
	{"pal8v4", "pal8"},
	{"pal8v5", "pal8"},
	{"rgb16", "rgb16"},
	{"rgb16bfdef", "rgb16"},
	{"rgb16-565", "rgb16-565"},
	{"rgb16-565pal", "rgb16-565"},
	{"rgb24", "rgb24"},
	{"rgb24pal", "rgb24"},
	{"rgb32", "rgb24"},
	{"rgb32bfdef", "rgb24"},
	{"rgb32bf", "rgb24"}
};

static const char *bad_files[] = {
	"badbitcount",
	"badbitssize",
	"baddens1",
	"baddens2",
	"badfilesize",
	"badheadersize",
	"badpalettesize",
	"badplanes",
	"badrle",
	"badrle4",
	"badrle4bis",
	"badrle4ter",
	"badrlebis",
	"badrleter",
	"badwidth",
	"pal8badindex",
	"reallybig",
	"rgb16-880",
	"rletopdown",
	"shortfile"
};

static const bmpsuite_good_t questionable_exact_files[] = {
	{"pal1p1", "pal1p1"},
	{"pal2", "pal2"},
	{"pal2color", "pal2color"},
	{"pal8offs", "pal8"},
	{"pal8os2-hs", "pal8"},
	{"pal8os2-sz", "pal8"},
	{"pal8os2sp", "pal8"},
	{"pal8os2v2-16", "pal8"},
	{"pal8os2v2-40sz", "pal8"},
	{"pal8os2v2-sz", "pal8"},
	{"pal8os2v2", "pal8"},
	{"pal8oversizepal", "pal8"},
	{"rgb16-231", "rgb16-231"},
	{"rgb16faketrns", "rgb16"},
	{"rgb24largepal", "rgb24"},
	{"rgb24lprof", "rgb24"},
	{"rgb24prof", "rgb24"},
	{"rgb32-xbgr", "rgb24"},
	{"rgb32fakealpha", "rgb24"},
	{"rgb32h52", "rgb24"}
};

static const char *questionable_load_files[] = {
	"pal4rlecut",
	"pal4rletrns",
	"pal8rlecut",
	"pal8rletrns",
	"rgb16-3103",
	"rgb24prof2",
	"rgb32-111110",
	"rgb32-7187",
	"rgba16-1924",
	"rgba16-4444",
	"rgba16-5551",
	"rgba32-1",
	"rgba32-1010102",
	"rgba32-2",
	"rgba32-61754",
	"rgba32-81284",
	"rgba32abf",
	"rgba32h56"
};

static const char *questionable_reject_files[] = {
	"pal1huffmsb",
	"rgb24jpeg",
	"rgb24png",
	"rgb24rle24",
	"rgba64"
};

static char *bmpsuite_path(const char *dir, const char *name, const char *ext)
{
	char filename[128];

	snprintf(filename, sizeof(filename), "%s.%s", name, ext);
	return gdTestFilePathX("bmp", "bmpsuite", dir, filename, NULL);
}

static gdImagePtr read_bmp(const char *path)
{
	FILE *fp;
	gdImagePtr im;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		return NULL;
	}
	im = gdImageCreateFromBmp(fp);
	fclose(fp);
	return im;
}

static gdImagePtr read_png(const char *path)
{
	FILE *fp;
	gdImagePtr im;

	fp = fopen(path, "rb");
	if (fp == NULL) {
		return NULL;
	}
	im = gdImageCreateFromPng(fp);
	fclose(fp);
	return im;
}

int main(void)
{
	size_t i;

	for (i = 0; i < sizeof(good_files) / sizeof(good_files[0]); i++) {
		char *bmp_path = bmpsuite_path("good", good_files[i].bmp, "bmp");
		char *png_path = bmpsuite_path("good", good_files[i].png, "png");
		gdImagePtr actual = read_bmp(bmp_path);
		gdImagePtr expected = read_png(png_path);
		CuTestImageResult result = {0, 0};

		gdTestAssertMsg(actual != NULL, "failed to read BMPSuite BMP %s\n", bmp_path);
		gdTestAssertMsg(expected != NULL, "failed to read BMPSuite reference %s\n", png_path);
		if (actual != NULL && expected != NULL) {
			gdTestImageDiff(expected, actual, NULL, &result);
			gdTestAssertMsg(result.pixels_changed == 0,
				"BMPSuite BMP %s differs from %s: %u pixels changed, max diff %u\n",
				bmp_path, png_path, result.pixels_changed, result.max_diff);
		}

		if (actual != NULL) {
			gdImageDestroy(actual);
		}
		if (expected != NULL) {
			gdImageDestroy(expected);
		}
		free(bmp_path);
		free(png_path);
	}

	for (i = 0; i < sizeof(bad_files) / sizeof(bad_files[0]); i++) {
		char *bmp_path = bmpsuite_path("bad", bad_files[i], "bmp");
		gdImagePtr actual = read_bmp(bmp_path);

		gdTestAssertMsg(actual == NULL, "invalid BMPSuite BMP decoded successfully: %s\n", bmp_path);
		if (actual != NULL) {
			gdImageDestroy(actual);
		}
		free(bmp_path);
	}

	for (i = 0; i < sizeof(questionable_exact_files) / sizeof(questionable_exact_files[0]); i++) {
		char *bmp_path = bmpsuite_path("questionable", questionable_exact_files[i].bmp, "bmp");
		char *png_path = bmpsuite_path("questionable", questionable_exact_files[i].png, "png");
		gdImagePtr actual = read_bmp(bmp_path);
		gdImagePtr expected = read_png(png_path);
		CuTestImageResult result = {0, 0};

		gdTestAssertMsg(actual != NULL, "failed to read questionable BMPSuite BMP %s\n", bmp_path);
		gdTestAssertMsg(expected != NULL, "failed to read questionable BMPSuite reference %s\n", png_path);
		if (actual != NULL && expected != NULL) {
			gdTestImageDiff(expected, actual, NULL, &result);
			gdTestAssertMsg(result.pixels_changed == 0,
				"questionable BMPSuite BMP %s differs from %s: %u pixels changed, max diff %u\n",
				bmp_path, png_path, result.pixels_changed, result.max_diff);
		}

		if (actual != NULL) {
			gdImageDestroy(actual);
		}
		if (expected != NULL) {
			gdImageDestroy(expected);
		}
		free(bmp_path);
		free(png_path);
	}

	for (i = 0; i < sizeof(questionable_load_files) / sizeof(questionable_load_files[0]); i++) {
		char *bmp_path = bmpsuite_path("questionable", questionable_load_files[i], "bmp");
		gdImagePtr actual = read_bmp(bmp_path);

		gdTestAssertMsg(actual != NULL, "failed to safely read questionable BMPSuite BMP %s\n", bmp_path);
		if (actual != NULL) {
			gdImageDestroy(actual);
		}
		free(bmp_path);
	}

	for (i = 0; i < sizeof(questionable_reject_files) / sizeof(questionable_reject_files[0]); i++) {
		char *bmp_path = bmpsuite_path("questionable", questionable_reject_files[i], "bmp");
		gdImagePtr actual = read_bmp(bmp_path);

		gdTestAssertMsg(actual == NULL, "unsupported questionable BMPSuite BMP decoded successfully: %s\n", bmp_path);
		if (actual != NULL) {
			gdImageDestroy(actual);
		}
		free(bmp_path);
	}

	return gdNumFailures();
}
