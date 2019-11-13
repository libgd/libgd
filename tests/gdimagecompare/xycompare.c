/**
 * Basic test for gdImageCompare()
 */
#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr im1, im2;
	int cmpStatus = 0, result = 0;

	im1 = gdImageCreate(120, 105);
	im2 = gdImageCreate(100, 100);

	if (im1 == NULL || im2 == NULL) {
		gdTestErrorMsg("could not create image\n");
		result = 1;
		goto done;
	}

	//im1->sx != im2->sx   im1->sy != im2->sy
	cmpStatus = gdImageCompare(im1, im2);
	if (cmpStatus == (GD_CMP_SIZE_X + GD_CMP_SIZE_Y + GD_CMP_IMAGE)){
		result = 0;
		goto done;
	}

done:
	gdImageDestroy(im1);
	gdImageDestroy(im2);
	return result;
}
