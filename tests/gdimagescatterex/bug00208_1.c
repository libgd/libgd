#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	FILE *fp;
	gdScatter s;

	fp = fopen(GDTEST_TOP_DIR "/gdimagescatterex/bug00208.png", "rb");
	if (!fp) {
		fprintf(stderr, "could not open file");
		return 1;
	}
	im = gdImageCreateFromPng(fp);
	fclose(fp);
	if (!im) {
		fprintf(stderr, "could not create image");
		return 1;
	}

	s.sub  = 1;
	s.plus = 3;
	s.seed = 0;
	s.num_colors = 0;
	if (!gdImageScatterEx(im, &s)) {
		gdImageDestroy(im);
		fprintf(stderr, "could not scatter");
		return 1;
	}

	if (!gdAssertImageEqualsToFile(GDTEST_TOP_DIR "/gdimagescatterex/bug00208_1.png", im)) {
		gdImageDestroy(im);
		return 1;
	}
	return 0;
}
