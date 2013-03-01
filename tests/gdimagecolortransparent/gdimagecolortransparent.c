#include "gd.h"
#include <stdio.h>
#include "gdtest.h"

int main()
{
	gdImagePtr im;
	int error = 0, pos;

	pos = gdMaxColors;

	im = gdImageCreate(1,1);

	gdImageColorTransparent(im, pos);

	if(im->transparent == pos) {
		error = -1;
	}

	pos = -2;

	gdImageColorTransparent(im, pos);

	if(im->transparent == pos) {
		error = -1;
	}

	gdImageDestroy(im);

	return error;
}
