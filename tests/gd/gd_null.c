#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromGd(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageGd(im, NULL); /* noop safely */
	return 0;
}
