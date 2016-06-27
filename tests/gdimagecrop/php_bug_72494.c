#include <stdio.h>
#include <stdlib.h>
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr im, exp;
	int error = 0;

	im = gdImageCreate(50, 50);

	if (!im) {
		gdTestErrorMsg("gdImageCreate failed.\n");
		return 1;
	}

	gdImageCropThreshold(im, 1337, 0);
	gdImageDestroy(im);
	/* this bug tests a crash, it never reaches this point if the bug exists*/
	return 0;
}
