#include <limits.h>
#include "gd.h"
#include "gdhelpers.h"
#include "gdtest.h"


int main()
{
	gdImagePtr im;

	im = gdImageCreateTrueColor(400,300);
	if (im == NULL) {
		gdTestErrorMsg("image creation failed.\n");
		return 1;
	}
    int a = INT_MAX;
    int b = INT_MAX;

	gdImageEllipse(im, 64, 150, INT_MAX, INT_MAX, gdImageColorAllocate(im, 150, 255, 0));
    int p = gdImageGetPixel(im, 64, 150);
    gdTestAssertMsg(p==0, "Expected pixel to be unchanged when drawing ellipse with dimensions that would cause overflow\n");
    gdImageFilledEllipse(im, 64, 150, INT_MAX, INT_MAX, gdImageColorAllocate(im, 150, 255, 0));
	gdImageDestroy(im);
	return 0;
}
