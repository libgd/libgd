#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromTiff(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageTiff(im, NULL); /* noop safely */
	return 0;
}
