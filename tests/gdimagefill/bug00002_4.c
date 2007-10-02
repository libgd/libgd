/*ImageFill Pattern fill */

#include "gd.h"
#include "gdtest.h"

int main()
{
 	gdImagePtr im;
	FILE *fp;
	int red, blue, white, black, error = 0;
	char path[1024];

	im = gdImageCreate(50,100);
	red = gdImageColorAllocate(im, 255, 0, 0);
	blue = gdImageColorAllocate(im, 0,0,255);
	white  = gdImageColorAllocate(im, 255,255,255);
	black = gdImageColorAllocate(im, 0,0,0);
	gdImageFill(im, 0,0, black);

	gdImageLine(im, 20,20,180,20, white);
	gdImageLine(im, 20,20,20,70, blue);
	gdImageLine(im, 20,70,180,70, red);
	gdImageLine(im, 180,20,180,45, white);
	gdImageLine(im, 180,70,180,45, red);
	gdImageLine(im, 20,20,100,45, blue);
	gdImageLine(im, 20,70,100,45, blue);
	gdImageLine(im, 100,45,180,45, red);

	gdImageFill(im, 21,45, blue);
	gdImageFill(im, 100,69, red);
	gdImageFill(im, 100,21, white);

	sprintf(path, "%s/gdimagefill/bug00002_4_exp.png", GDTEST_TOP_DIR);
	if (!gdAssertImageEqualsToFile(path, im)) {
		error = 1;
	}

 	/* Destroy it */
 	gdImageDestroy(im);
 	return error;
}
