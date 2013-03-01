#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromWBMP(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageWBMP(im, 0, NULL); /* noop safely */
	return 0;
}
