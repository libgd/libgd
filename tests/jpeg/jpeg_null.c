#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromJpeg(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageJpeg(im, NULL, 100); /* noop safely */
	return 0;
}
