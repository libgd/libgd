#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im, ex;
	FILE *fp;
	gdScatter s;
	int colors[] = {0xFF0000, 0x00FF00};
	CuTestImageResult r;

	fp = gdTestFileOpen("gdimagescatterex/bug00208.png");
	im = gdImageCreateFromPng(fp);
	fclose(fp);
	if (!im) {
		gdTestErrorMsg("could not create image\n");
		return 1;
	}

	s.sub  = 1;
	s.plus = 3;
	s.seed = 0;
	s.num_colors = 2;
	s.colors = colors;
	if (!gdImageScatterEx(im, &s)) {
		gdImageDestroy(im);
		gdTestErrorMsg("could not scatter\n");
		return 1;
	}

	fp = gdTestFileOpen("gdimagescatterex/bug00208_2.png");
	ex = gdImageCreateFromPng(fp);
	fclose(fp);
	if (!ex) {
		gdTestErrorMsg("could not create image\n");
		gdImageDestroy(im);
		return 1;
	}
	r.pixels_changed = 0;
	gdTestImageDiff(im, ex, NULL, &r);
	gdImageDestroy(ex);
	gdImageDestroy(im);
	if (r.pixels_changed > 10000) {
		gdTestErrorMsg("too much diff: %d\n", r.pixels_changed);
		return 1;
	}
	return 0;
}
