#include <stdio.h>
#include "gd.h"
#include "gdtest.h"

int main() {
	gdImagePtr im;

	im = gdImageCreate(50, 50);
	gdImageFillToBorder(im, 0, 0, 1024, 1024);
	gdImageDestroy(im);
	return 0;
}
