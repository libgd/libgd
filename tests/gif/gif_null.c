#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromGif(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageGif(im, NULL); /* noop safely */
	return 0;
}
