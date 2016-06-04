#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main() {
	gdImagePtr im;
	int  white, red;

	im = gdImageCreate(50, 50);
	white = gdImageColorAllocate(im, 255, 255, 255);
	red = gdImageColorAllocate(im, 255, 0, 0);
	gdImageFillToBorder(im, 0, 0, 1024, 1024);
	gdImageDestroy(im);
	return 0;
}
