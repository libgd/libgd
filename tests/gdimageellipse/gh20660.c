#include "gd.h"
#include "gdtest.h"
#include <limits.h>

int main()
{
	gdImagePtr im;

	im = gdImageCreateTrueColor(1024, 1024);
	gdImageEllipse(im, 5024, 5024, 1234567890 >> 1, 254, 0);
	gdImageDestroy(im);

	return 0;
}
