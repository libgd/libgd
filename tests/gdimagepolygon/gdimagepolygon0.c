#include <stdlib.h>
#include "gd.h"
#include "gdtest.h"

int
main(void)
{
	gdImagePtr im;
	int white, black, r;

	im = gdImageCreate(100, 100);
	if (!im) exit(EXIT_FAILURE);
	white = gdImageColorAllocate(im, 0xff, 0xff, 0xff);
	black = gdImageColorAllocate(im, 0, 0, 0);
	gdImageFilledRectangle(im, 0, 0, 99, 99, white);
	gdImagePolygon(im, NULL, 0, black);  /* no effect */
	gdImagePolygon(im, NULL, -1, black); /* no effect */
	r = gdAssertImageEqualsToFile("gdimagepolygon/gdimagepolygon0.png", im);
	gdImageDestroy(im);
	if (!r) exit(EXIT_FAILURE);
	return EXIT_SUCCESS;
}
