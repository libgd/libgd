/**
 * Basic test for gdImageCompare()
 */
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im1, im2;
	int cmpStatus = 0, result = 0;

	im1 = gdImageCreate(100, 100);
	im2 = gdImageCreate(100, 100);

	if (im1 == NULL || im2 == NULL) {
		gdTestErrorMsg("could not create image\n");
		result = 1;
		goto done;
	}

	//im1 == im2
	cmpStatus = gdImageCompare(im1, im2);
	if (cmpStatus != 0) {
		result = 1;
		goto done;
	}

	//im1->colorsTotal != im2->colorsTotal
	gdImageColorAllocate(im1, 206, 0, 0);
	cmpStatus = gdImageCompare(im1, im2);
	if (cmpStatus == (GD_CMP_NUM_COLORS + GD_CMP_COLOR + GD_CMP_IMAGE)){
		result = 0;
		goto done;
	}

done:
	gdImageDestroy(im1);
	gdImageDestroy(im2);
	return result;
}
