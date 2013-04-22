#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im;

	gdSetErrorMethod(gdSilence);

	im = gdImageCreateFromPng(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImagePng(im, NULL); /* noop safely */
	return 0;
}
