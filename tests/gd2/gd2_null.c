#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromGd2(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageGd2(im, NULL, 0, GD2_FMT_RAW); /* noop safely */
	return 0;
}
