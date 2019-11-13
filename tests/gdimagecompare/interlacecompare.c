/**
 * Basic test for gdImageCompare()
 */
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im1, im2;
	int r1, r2, cmpStatus = 0, result = 0;

	im1 = gdImageCreate(100, 100);
	im2 = gdImageCreate(100, 100);

	if (im1 == NULL || im2 == NULL) {
		gdTestErrorMsg("could not create image\n");
		result = 1;
		goto done;
	}

	r1 = gdImageColorAllocate(im1, 205, 0, 0);
	r2 = gdImageColorAllocate(im2, 205, 0, 0);
	gdImageRectangle(im1, 20, 20, 79, 79, r1);
	gdImageRectangle(im2, 20, 20, 79, 79, r2);

	//im1 == im2
	cmpStatus = gdImageCompare(im1, im2);
	if (cmpStatus != 0) {
		result = 1;
		goto done;
	}

	//im1->interlace != im2->interlace
	gdImageInterlace(im1, 1);
	gdImageInterlace(im2, 2);
	cmpStatus = gdImageCompare(im1, im2);
	if (cmpStatus == GD_CMP_INTERLACE){
		result = 0;
		goto done;
	}

done:
	gdImageDestroy(im1);
	gdImageDestroy(im2);
	return result;
}
