#include "gd.h"

int main()
{
	gdImagePtr im;

	im = gdImageCreateFromTga(NULL);
	if (im != NULL) {
		gdImageDestroy(im);
		return 1;
	}
	return 0;
}
