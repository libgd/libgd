#include "gd.h"
#include "gdtest.h"

int main()
{
	gdImagePtr dst = NULL;
	gdImagePtr src = NULL;
	//FILE *pngout = NULL;
	int error = 0;

	dst = gdImageCreate(100, 100);
	src = gdImageCreate(50, 50);

	if (!gdTestAssert(dst != NULL)) {
		error = 1;
		goto done;
	}
	if (!gdTestAssert(src != NULL)) {
		error = 1;
		goto done;
	}

	gdImageColorAllocate(src, 255, 255, 255);
	gdImageColorAllocate(dst, 0, 0, 0);
	gdImageCopyMergeGray(dst, src, 25, 25, 0, 0, 50, 50, 50);

	/*Code to generate expected image*/
	//pngout = fopen("gdimagecopymergegray_exp.png", "wb");
	//gdImagePng(dst, pngout);
	//fclose(pngout);

	if (!gdAssertImageEqualsToFile("gdimagecopymergegray/gdimagecopymergegray_exp.png", dst)) {
		error = 1;
	}

done:
	gdImageDestroy(dst);
	gdImageDestroy(src);

	return error;
}
