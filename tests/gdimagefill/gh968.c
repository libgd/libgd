#include "gd.h"
#include "gdtest.h"

int main(void)
{
	gdImagePtr im;

	im = gdImageCreate(2, 150);
	gdImageFill(im, 11, 12, gdTiled);
	gdImageDestroy(im);
	return 0;
}
