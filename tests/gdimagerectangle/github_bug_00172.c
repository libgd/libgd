#include <stdio.h>
#include "gd.h"
#include "gdfontg.h"
#include "gdtest.h"

int main() {
	gdImagePtr im, imres, imexp;
	int white, black, red;
	char path[1024];
	int error = 0;

	im = gdImageCreate(400, 200);

	if (im == NULL) {
		gdTestErrorMsg("gdImageCreate failed.\n");
		return 1;
	}

	white = gdImageColorAllocate(im, 255,255,255);
	black = gdImageColorAllocate(im, 0,0,0);
	red = gdImageColorAllocate(im, 255,0,0);
	gdImageString(im, gdFontGetGiant(), 5, 5, "RECTANGLES", black);
	gdImageString(im, gdFontGetGiant(), 150, 5, "Filled", black);
	gdImageString(im, gdFontGetGiant(), 300, 5, "Unfilled", black);
	gdImageString(im, gdFontGetGiant(), 5, 75, "Height=0", black);
	gdImageString(im, gdFontGetGiant(), 5, 145, "Width=0", black);
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
