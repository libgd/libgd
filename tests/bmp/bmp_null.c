#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromBmp(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageBmp(im, NULL, 0); /* noop safely */
	return 0;
}
