#include "gd.h"
#include "gdtest.h"

typedef struct {
	const char *path;
	int width;
	int height;
} TiffDepthCase;

static void assert_tiff_readable(const TiffDepthCase *tc)
{
	FILE *fp;
	gdImagePtr im;

	fp = gdTestFileOpen2("tiff", tc->path);
	gdTestAssertMsg(fp != NULL, "cannot open %s", tc->path);
	if (fp == NULL) {
		return;
	}

	im = gdImageCreateFromTiff(fp);
	fclose(fp);

	gdTestAssertMsg(im != NULL, "cannot read %s", tc->path);
	if (im == NULL) {
		return;
	}

	gdTestAssertMsg(gdImageSX(im) == tc->width && gdImageSY(im) == tc->height,
	                "%s decoded to %dx%d, expected %dx%d",
	                tc->path, gdImageSX(im), gdImageSY(im), tc->width, tc->height);
	gdTestAssertMsg(gdImageTrueColor(im), "%s should decode through the RGBA fallback", tc->path);
	gdImageDestroy(im);
}

int main(void)
{
	static const TiffDepthCase cases[] = {
		{"flower-minisblack-16.tif", 73, 43},
		{"flower-rgb-contig-16.tif", 73, 43},
		{"flower-rgb-planar-16.tif", 73, 43},
	};
	size_t i;

	for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
		assert_tiff_readable(&cases[i]);
	}

	return gdNumFailures();
}
