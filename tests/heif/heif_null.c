#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromHeif(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	gdImageHeif(im, NULL); /* noop safely */
	return 0;
}
