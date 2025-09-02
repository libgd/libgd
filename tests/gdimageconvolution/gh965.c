#include "gd.h"
#include "gdtest.h"
#include <math.h>

int main()
{
	gdImagePtr im;
	float matrix[3][3] = {
		{1.0, 2.0, 1.0},
		{2.0, 4.0, 2.0},
		{1.0, 2.0, -INFINITY}
	};

	im = gdImageCreateTrueColor(100, 30);

	int r = gdImageConvolution(im, matrix, 16, 0);
	gdImageDestroy(im);
	gdTestAssertMsg(r == 0, "GD Warning: Matrix: invalid top-left filter");
	float matrix2[3][3] = {
		{1.0, 2.0, 1.0},
		{2.0, 4.0, 2.0},
		{1.0, 2.0, INFINITY}
	};
	im = gdImageCreateTrueColor(100, 30);
	r = gdImageConvolution(im, matrix2, 16, 0);
	gdImageDestroy(im);
	gdTestAssertMsg(r == 0, "GD Warning: Matrix: invalid top-left filter");

	return 0;
}

