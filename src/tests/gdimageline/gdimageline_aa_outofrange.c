#include <gd.h>
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int bordercolor, color;

	im = gdImageCreateTrueColor(300, 300);

   gdImageSetAntiAliased(im, gdTrueColorAlpha(255, 255, 255, 0));

   gdImageLine(im, -1, -1, -1, -1, gdAntiAliased);
   gdImageLine(im, 299, 299, 0, 299, gdAntiAliased);
	gdImageLine(im, 1,1, 50, 50, gdAntiAliased);

	/* Test for segfaults, if we reach this point, the test worked */
	return 0;
}
