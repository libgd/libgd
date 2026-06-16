#include "gd.h"
#include "gdtest.h"

int main() {
	FILE *fp;
	gdImagePtr im;

	fp = gdTestFileOpen("jxl/flower_small_rgb.jxl");
	gdTestAssert(fp != NULL);

	im = gdImageCreateFromJxl(fp);
	fclose(fp);

	gdTestAssert(im != NULL);
	gdTestAssert(gdImageSX(im) > 0);
	gdTestAssert(gdImageSY(im) > 0);

	gdImageDestroy(im);

	return gdNumFailures();
}
