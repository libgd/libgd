#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main() {
	gdImagePtr im;
	int color, border;
	im = gdImageCreate(50, 50);
	gdImageColorAllocate(im, 255, 0, 0);
	color = gdImageColorAllocate(im, 255, 0, 0);
	border = gdImageColorAllocate(im, 255, 0, 0);
	gdImageFillToBorder(im, 0, 0, border + 10, color);
	gdImageFillToBorder(im, 0, 0, -border, color);
	gdImageFillToBorder(im, 0, 0, border, color + 10);
	gdImageFillToBorder(im, 0, 0, border, -color);
	gdImageFillToBorder(im, 0, 0, border + 10, color + 10);
	gdImageDestroy(im);
	return 0;
}
