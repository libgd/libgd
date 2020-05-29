/**
 * Basic test for gdImageColorMatch()
 **/
#include "gd.h"
#include "gdtest.h"

int main(){
	gdImagePtr im1, im2;

	im1 = gdImageCreateTrueColor(80, 80);
	im2 = gdImageCreate(80, 80);

	gdImageColorAllocate(im1, 255, 36, 74);

	gdImageColorAllocate(im2, 255, 36, 74);
	gdImageColorAllocate(im2, 40, 0, 240);
	gdImageColorAllocate(im2, 255, 100, 255);
	gdImageColorAllocate(im2, 80, 60, 44);

	int ifMatch = gdImageColorMatch(im1, im2);

	gdImageDestroy(im1);
	gdImageDestroy(im2);

	if (gdTestAssert(ifMatch != 0))
	{
		gdTestErrorMsg("gdImageColorMatch failed.\n");
		return 1;
	}

	return 0;
}
