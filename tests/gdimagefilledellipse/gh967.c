#include "gd.h"
#include "gdtest.h"
#include <limits.h>

int main()
{
	gdImagePtr im;

	im = gdImageCreateTrueColor(400,300);
	if (im == NULL) {
		gdTestErrorMsg("image creation failed.\n");
		return 1;
	}

	gdImageFilledEllipse(im, 64, 150, 300, ULONG_MAX, gdImageColorAllocate(im, 150, 255, 0));
	gdImageDestroy(im);
	return 0;
}

