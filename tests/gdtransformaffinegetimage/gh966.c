#include "gd.h"

#include "gdtest.h"
#include <limits.h>


int main()
{
		gdImagePtr im, d;
		gdRect rect;
		double affine[6];

		affine[0] = INT_MAX;
		affine[1] = 1;
		affine[2] = 1;
		affine[3] =  1;
		affine[4] =  1;
		affine[5] =  1;

		im = gdImageCreateTrueColor(8, 8);
		gdTransformAffineGetImage(&d, im, &rect, affine);

		return 0;
}

