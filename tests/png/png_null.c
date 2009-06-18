#include <gd.h>

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromPng(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImagePng(im, NULL); /* noop safely */
	return 0;
}
