/* $Id: bug00010.c 233569 2007-04-10 20:32:37Z pajoye $ */
#include "gd.h"
#include "gdtest.h"

int main()
{
 	gdImagePtr im;
 	int error = 0;
	char path[1024];

	im = gdImageCreateTrueColor(100,100);
	gdImageFilledEllipse(im, 50,50, 70, 90, 0x50FFFFFF);

	sprintf(path, "%s/gdimagefilledellipse/bug00010_exp.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
	}

 	gdImageDestroy(im);
 	return error;
}
