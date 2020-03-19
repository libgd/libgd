#include <stdio.h>
#include <stdlib.h>
#include "gd.h"
#include <math.h>

#include "gdtest.h"

int main()
{
	gdImagePtr im, im2;

	im = gdImageCreate(1,1);
	if (im == NULL) {
		printf("gdImageCreate failed\n");
		return 1;
	}
	gdImageSetInterpolationMethod(im, GD_BELL);

	/* here the call may pass if the system has enough memory (physical or swap)
	   or fails (overflow check or alloc fails.
	   in both cases the tests pass */
	im2 = gdImageScale(im,0x15555556, 1);
	if (im2 == NULL) {
		printf("gdImageScale failed, expected (out of memory or overflow validation\n");
		gdImageDestroy(im);
		return 0;
	}
	gdImageDestroy(im);
	gdImageDestroy(im2);

	return 0;
}
