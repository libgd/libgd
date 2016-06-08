#include <stdio.h>
#include "gd.h"
#include "gdfontg.h"
#include "gdtest.h"

int main() {
	gdImagePtr im;
	int black, red;
	char path[1024];
	int error = 0;

	im = gdImageCreate(400, 200);

	if (im == NULL) {
		gdTestErrorMsg("gdImageCreate failed.\n");
		return 1;
	}

	gdImageColorAllocate(im, 255,255,255);
	black = gdImageColorAllocate(im, 0,0,0);
	red = gdImageColorAllocate(im, 255,0,0);
	gdImageString(im, gdFontGetGiant(), 5, 5,
	              (unsigned char *) "RECTANGLES", black);
	gdImageString(im, gdFontGetGiant(), 150, 5,
	              (unsigned char *) "Filled", black);
	gdImageString(im, gdFontGetGiant(), 300, 5,
	              (unsigned char *) "Unfilled", black);
	gdImageString(im, gdFontGetGiant(), 5, 75,
	              (unsigned char *) "Height=0", black);
	gdImageString(im, gdFontGetGiant(), 5, 145,
	              (unsigned char *) "Width=0", black);
	gdImageFilledRectangle(im, 150, 95, 200, 95, black);
	gdImageRectangle(im, 300, 95, 350, 95, black);
	gdImageSetPixel(im, 300, 94, red);
	gdImageSetPixel(im, 350, 94, red);

	gdImageFilledRectangle(im, 175, 125, 175, 175, black);
	gdImageRectangle(im, 325, 125, 325, 175, black);
	gdImageSetPixel(im, 300, 94, red);
	gdImageSetPixel(im, 350, 94, red);
	gdImageEllipse(im, 325, 95, 90, 50, red);



	sprintf(path, "%s/gdimagerectangle/%s", GDTEST_TOP_DIR, "github_bug_00172_exp.png");
	if (!gdAssertImageEqualsToFile(path, im)) {
		error |= 1;
	}
	gdImageDestroy(im);

	return error;
}
