/**
  * Basic test for gdImageCopyResized()
  **/
#include "gd.h"

#include "gdtest.h"

int main()
{
	gdImagePtr dst_tc, src;
	int c1;
	FILE *file1, *file2;
	int status = 0;

	src = gdImageCreate(25,25);
	gdImageAlphaBlending(src, 0);

	gdImageColorAllocate(src, 255,255,255);
	c1 = gdImageColorAllocateAlpha(src, 255,0,0,70);

	gdImageFilledRectangle(src, 0,0, 4,4, c1);

	dst_tc  = gdImageCreateTrueColor(55,55);
	gdImageAlphaBlending(dst_tc, 0);
	gdImageCopyResized(dst_tc, src, 0, 0, 0, 0, 25, 25, gdImageSX(src), gdImageSY(src));

	file1 = gdTestFileOpen2("gdimagecopyresized", "gdimagecopyresized.png");
	file2 = gdTestFileOpen2("gdimagecopyresized", "gdimagecopyresized_exp.png");

	gdImagePng(src, file1);
	gdImagePng(dst_tc, file2);

	fclose(file1);
	fclose(file2);

	gdImageDestroy(dst_tc);
	gdImageDestroy(src);
	return status;
}
