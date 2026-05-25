#include "gd.h"

#include "gdtest.h"
#include <limits.h>


int main()
{
		gdImagePtr im, d = NULL;
		gdRect bbox;
		gdRect rect;
		double affine[6];

		affine[0] = INT_MAX;
		affine[1] = 1;
		affine[2] = 1;
		affine[3] =  1;
		affine[4] =  1;
		affine[5] =  1;

		rect.x = 0;
		rect.y = 0;
		rect.width = 8;
		rect.height = 8;

		gdTestAssert(gdTransformAffineBoundingBox(&rect, affine, &bbox) == GD_FALSE);

		im = gdImageCreateTrueColor(8, 8);
		if (gdTestAssert(im != NULL)) {
			gdTestAssert(gdTransformAffineGetImage(&d, im, &rect, affine) == GD_FALSE);
			gdTestAssert(d == NULL);
			gdImageDestroy(im);
		}

		return gdNumFailures();
}
