#include <gd.h>
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0;

	im = gdImageCreate(1, 1);

	im->open[gdMaxColors] = 0;

	gdImageColorDeallocate(im, gdMaxColors);

	if(im->open[gdMaxColors] == 1) {
		error = -1;
	}	

	gdImageDestroy(im);

	return error;
}
